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
#include <asterkv/server/tcp_server_options.h>
#include <asterkv/server/tcp_server_runtime.h>

namespace {
    void printUsage(std::string_view execName) {
        std::cout
            << "Usage: " << execName << " [--version] [--help]\n"
            << "       " << execName << " --local\n"
            << "       " << execName << " --local <command>\n"
            << "       " << execName << " --listen <host:port> [--max-clients <count>] [--idle-timeout <seconds>]\n\n"
            << "AsterKV server daemon.\n\n"
            << "Options:\n"
            << "    --version                   Print version information.\n"
            << "    --help                      Print this help message.\n"
            << "    --local                     Run local stdin command mode without TCP networking\n"
            << "    --listen <host:port>        Run blocking sequential TCP line server until Ctrl+C\n"
            << "    --max-clients <count>       Limit active TCP client workers\n"
            << "    --idle-timeout <seconds>    Close idle TCP clients after timeout\n\n"
            << "Examples:\n"
            << "  " << execName << " --local PING\n"
            << "  " << execName << " --local \"SET username alex\"\n"
            << "  printf 'PING'\\n | " << execName << " --local\n"
            << " " << execName << " --listen 127.0.0.1:" << AsterKV::Network::defaultClientPort << '\n'
            << " " << execName << " --listen 127.0.0.1:" << AsterKV::Network::defaultClientPort << " --max-clients 128 --idle timeout 300\n"
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

    [[nodiscard]] AsterKV::Core::Result<std::size_t> parsePositiveSize(std::string_view value, std::string_view err) {
        std::size_t parsedValue = 0;

        const char* const begin = value.data();
        const char* const end = value.data() + value.size();

        const auto parseResult = std::from_chars(begin, end, parsedValue);

        if (parseResult.ec != std::errc {} || parseResult.ptr != end || parsedValue == 0) {
            return AsterKV::Core::Result<std::size_t>::failure(
                AsterKV::Core::Status::invalidArgument(std::string {err})
            );
        }

        return AsterKV::Core::Result<std::size_t>::success(parsedValue);
    }

    [[nodiscard]] AsterKV::Core::Result<std::uint32_t> parseIdleTimeoutSeconds(std::string_view value) {
        auto parsedValue = parsePositiveSize(value, "idle timeout must be a positive integer");
        if (parsedValue.isError()) {
            return AsterKV::Core::Result<std::uint32_t>::failure(parsedValue.status());
        }

        if (parsedValue.value() > std::numeric_limits<std::uint32_t>::max()) {
            return AsterKV::Core::Result<std::uint32_t>::failure(
                AsterKV::Core::Status::invalidArgument("idle timeout is too large")
                );
        }

        return AsterKV::Core::Result<std::uint32_t>::success(static_cast<std::uint32_t>(parsedValue.value()));
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

    int runListenMode(const AsterKV::Server::TcpServerOptions &options) {
        AsterKV::Server::TcpServerRuntime runtime {options};

        std::cout << "AsterKV listening on " << AsterKV::Network::tcpEndpointToString(runtime.endpoint()) << '\n'
            << "Max client workers: " << runtime.options().maxClientWorkers << '\n'
            << "Client idle timeout seconds: " << runtime.options().clientIdleTimeoutSeconds << '\n'
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
        if (argc < 3) {
            std::cerr << "--listen requires <host:port>\n";

            return EXIT_FAILURE;
        }

        auto endpoint = AsterKV::Network::parseTcpEndpoint(argv[2]);
        if (endpoint.isError()) {
            std::cerr << "Invalid listen address: " << endpoint.status().message() << '\n';

            return EXIT_FAILURE;
        }

        AsterKV::Server::TcpServerOptions options = AsterKV::Server::defaultServerOptions();
        options.endpoint = endpoint.value();
        int index = 3;

        while (index < argc) {
            const std::string_view optionName = argv[index];

            if (optionName == "--max-clients") {
                if (index + 1 >= argc) {
                    std::cerr << "--max-clients requires <count>\n";

                    return EXIT_FAILURE;
                }

                auto parsedMaxClients = parsePositiveSize(
                    argv[index + 1],
                    "max clients must be a positive integer"
                );

                if (parsedMaxClients.isError()) {
                    std::cerr << "Invalid max clients: " << parsedMaxClients.status().message() << '\n';

                    return EXIT_FAILURE;
                }

                options.maxClientWorkers = parsedMaxClients.value();
                index += 2;
                continue;
            }

            if (optionName == "--idle-timeout") {
                if (index + 1 >= argc) {
                    std::cerr << "--idle-timeout requires <seconds>\n";

                    return EXIT_FAILURE;
                }

                auto parsedIdleTimeout = parseIdleTimeoutSeconds(argv[index + 1]);
                if (parsedIdleTimeout.isError()) {
                    std::cerr << "Invalid idle timeout: " << parsedIdleTimeout.status().message() << '\n';

                    return EXIT_FAILURE;
                }

                options.clientIdleTimeoutSeconds = parsedIdleTimeout.value();
                index += 2;
                continue;
            }

            std::cerr << "Unknown listen option: " << optionName << '\n';

            return EXIT_FAILURE;
        }

        return runListenMode(options);
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
