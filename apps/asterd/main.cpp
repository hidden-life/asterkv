#include <cstdlib>
#include <sstream>
#include <iostream>
#include <string_view>
#include <csignal>
#include <cstring>

#include <asterkv/network/tcp_server.h>
#include <asterkv/pipeline/local_pipeline.h>
#include <asterkv/storage/in_memory_storage.h>
#include <asterkv/core/version.h>

#include "asterkv/server/tcp_server_options.h"
#include "asterkv/server/tcp_server_runtime.h"

namespace {
    void printUsage(std::string_view execName) {
        std::cout
            << "Usage: " << execName << " [--version] [--help]\n"
            << "       " << execName << " --local\n"
            << "       " << execName << " --local <command>\n"
            << "       " << execName << " --listen <host:port> [--max-clients <count>]\n\n"
            << "AsterKV server daemon.\n\n"
            << "Options:\n"
            << "    --version               Print version information.\n"
            << "    --help                  Print this help message.\n"
            << "    --local                 Run local stdin command mode without TCP networking\n"
            << "    --listen <host:port>    Run blocking sequential TCP line server until Ctrl+C\n"
            << "    --max-clients <count>   Limit active TCP client workers\n\n"
            << "Examples:\n"
            << "  " << execName << " --local PING\n"
            << "  " << execName << " --local \"SET username alex\"\n"
            << "  printf 'PING'\\n | " << execName << " --local\n"
            << " " << execName << " --listen 127.0.0.1:" << AsterKV::Network::defaultClientPort << '\n'
            << " " << execName << " --listen 127.0.0.1:" << AsterKV::Network::defaultClientPort << " --max-clients 128\n"
        ;
    }

    [[nodiscard]] std::string joinArguments(int argc, char **argv, int startIndex) {
        std::ostringstream stream;

        for (int index = startIndex; index < argc; ++index) {
            if (index > startIndex) {
                stream << ' ';
            }

            stream << argv[index];
        }

        return stream.str();
    }

    [[nodiscard]] AsterKV::Core::Result<std::size_t> parseMaxClientWorkers(std::string_view value) {
        std::size_t parsedValue = 0;

        const char* const begin = value.data();
        const char* const end = value.data() + value.size();

        const auto parseResult = std::from_chars(begin, end, parsedValue);

        if (parseResult.ec != std::errc {} || parseResult.ptr != end || parsedValue == 0) {
            return AsterKV::Core::Result<std::size_t>::failure(
                AsterKV::Core::Status::invalidArgument("max clients must be a positive integer")
            );
        }

        return AsterKV::Core::Result<std::size_t>::success(parsedValue);
    }

    void printProtocolResponse(std::string_view response) {
        std::cout << response;

        if (response.empty() || response.back() != '\n') {
            std::cout << '\n';
        }
    }

    int runLocalSingleCommand(int argc, char **argv) {
        AsterKV::Storage::InMemoryStorage storage;
        AsterKV::Pipeline::LocalPipeline pipeline {storage};

        const std::string cmdLine = joinArguments(argc, argv, 2);
        const std::string response = pipeline.processLine(cmdLine);

        printProtocolResponse(response);

        return EXIT_SUCCESS;
    }

    int runLocalStdinMode() {
        AsterKV::Storage::InMemoryStorage storage;
        AsterKV::Pipeline::LocalPipeline pipeline {storage};

        std::string line;

        while (std::getline(std::cin, line)) {
            if (line == "quit" || line == "exit") {
                return EXIT_SUCCESS;
            }

            if (line.empty()) {
                continue;
            }

            const std::string response = pipeline.processLine(line);
            printProtocolResponse(response);
        }

        return EXIT_SUCCESS;
    }

    int runLocalMode(int argc, char **argv) {
        if (argc == 2) {
            return runLocalStdinMode();
        }

        return runLocalSingleCommand(argc, argv);
    }

    int runListenMode(std::string_view endpointText, std::size_t maxClientWorkers) {
        auto endpoint = AsterKV::Network::parseTcpEndpoint(endpointText);

        if (endpoint.isError()) {
            std::cerr << "Invalid listen address: " << endpoint.status().message() << '\n';
            return EXIT_FAILURE;
        }

        AsterKV::Server::TcpServerOptions options {
            .endpoint = endpoint.value(),
            .maxClientWorkers = maxClientWorkers,
        };

        AsterKV::Server::TcpServerRuntime runtime {std::move(options)};

        std::cout << "AsterKV listening on " << AsterKV::Network::tcpEndpointToString(runtime.endpoint()) << '\n'
            << "Max client workers: " << runtime.options().maxClientWorkers << '\n'
            << "Press Ctrl+C to stop.\n" << std::flush;

        AsterKV::Core::Status status = runtime.run();
        if (!status.isOk()) {
            std::cerr << "TCP server failed: " << status.codeString() << ' ' << status.message() << '\n';

            return EXIT_FAILURE;
        }

        std::cout << "AsterKV server stopped.\n" << std::flush;

        return EXIT_SUCCESS;
    }

    int runListenModeFromArguments(int argc, char **argv) {
        if (argc != 3 && argc != 5) {
            std::cerr << "--listen requires <host:port>\n";

            return EXIT_FAILURE;
        }

        std::size_t maxClientWorkers = AsterKV::Network::defaultMaxClientWorkers;
        if (argc == 5) {
            const std::string_view optionName = argv[3];
            if (optionName != "--max-clients") {
                std::cerr << "Unknown listen options: " << optionName << '\n';

                return EXIT_FAILURE;
            }

            auto parsedMaxClients = parseMaxClientWorkers(argv[4]);

            if (parsedMaxClients.isError()) {
                std::cerr << "Invalid max clients: " << parsedMaxClients.status().message() << '\n';

                return EXIT_FAILURE;
            }

            maxClientWorkers = parsedMaxClients.value();
        }

        return runListenMode(argv[2], maxClientWorkers);
    }
}

int main(const int argc, char **argv) {
    const std::string_view execName = argc > 0 ? argv[0] : "asterd";

    if (argc == 1) {
        printUsage(execName);

        return EXIT_SUCCESS;
    }

    const std::string_view argument = argv[1];

    if (argument == "--version") {
        std::cout << AsterKV::Core::projectName() << " daemon " << AsterKV::Core::versionString() << '\n';

        return EXIT_SUCCESS;
    }

    if (argument == "--help") {
        printUsage(execName);

        return EXIT_SUCCESS;
    }

    if (argument == "--local") {
        return runLocalMode(argc, argv);
    }

    if (argument == "--listen") {
        return runListenModeFromArguments(argc, argv);
    }

    std::cerr << "Unknown argument: " << argument << std::endl;
    printUsage(execName);

    return EXIT_FAILURE;
}
