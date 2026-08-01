#include <cstdlib>
#include <sstream>
#include <iostream>
#include <string_view>

#include <asterkv/core/version.h>
#include <asterkv/network/tcp_endpoint.h>
#include <asterkv/pipeline/local_pipeline.h>
#include <asterkv/storage/in_memory_storage.h>
#include <asterkv/network/tcp_client.h>
#include <asterkv/client/response_renderer.h>

namespace {
    void printUsage(std::string_view execName) {
        std::cout
            << "Usage: " << execName << " [--version] [--help]\n"
            << "       " << execName << " --local <command>\n"
            << "       " << execName << " --connect <host:port> [command]\n\n"
            << "AsterKV command-line client.\n\n"
            << "Options:\n"
            << "    --version                   Print version information.\n"
            << "    --help                      Print this help message.\n"
            << "    --local <command>           Execute command against local in-memory pipeline\n"
            << "    --connect <host:port>       Execute command against TCP server\n\n"
            << "Modes:\n"
            << "    --connect with command      Execute one TCP command and exit\n"
            << "    --connect without command   Start TCP REPL mode\n\n"
            << "REPL commands:\n"
            << "    exit                        Exit REPL mode\n"
            << "    quit                        Exit REPL mode\n\n"
            << "Output:\n"
            << "    Successful responses are printed to stdout.\n"
            << "    Protocol errors are printed to stderr and return exit code 1.\n\n"
            << "Examples:\n"
            << "    " << execName << " --local PING\n"
            << "    " << execName << " --local \"SET username alex\"\n"
            << "    " << execName << " --connect 127.0.0.1:" << AsterKV::Network::defaultClientPort << " PING\n"
            << "    " << execName << " --connect 127.0.0.1:" << AsterKV::Network::defaultClientPort << " SET username alex\n"
            << "    " << execName << " --connect 127.0.0.1:" << AsterKV::Network::defaultClientPort << '\n'
        ;
    }

    [[nodiscard]] bool isWhitespace(char value) noexcept {
        return std::isspace(static_cast<unsigned char>(value)) != 0;
    }

    [[nodiscard]] std::string_view trim(std::string_view value) noexcept {
        while (!value.empty() && isWhitespace(value.front())) {
            value.remove_prefix(1);
        }

        while (!value.empty() && isWhitespace(value.back())) {
            value.remove_suffix(1);
        }

        return value;
    }

    [[nodiscard]] bool equalsIgnoreCase(std::string_view left, std::string_view right) noexcept {
        if (left.size() != right.size()) {
            return false;
        }

        for (std::size_t index = 0; index < left.size(); ++index) {
            const char leftCh = static_cast<char>(std::tolower(static_cast<unsigned char>(left[index])));
            const char rightCh = static_cast<char>(std::tolower(static_cast<unsigned char>(right[index])));

            if (leftCh != rightCh) {
                return false;
            }
        }

        return true;
    }

    [[nodiscard]] bool isExitCommand(std::string_view cmd) noexcept {
        const std::string_view trimmed = trim(cmd);

        return equalsIgnoreCase(trimmed, "exit") || equalsIgnoreCase(trimmed, "quit");
    }

    [[nodiscard]] std::string joinArguments(int argc, char **argv, int startIdx) {
        std::ostringstream stream;

        for (int index = startIdx; index < argc; ++index) {
            if (index > startIdx) {
                stream << ' ';
            }

            stream << argv[index];
        }

        return stream.str();
    }

    int printUserResponse(std::string_view raw) {
        auto parsed = AsterKV::Client::parseProtocolResponse(raw);
        if (parsed.isError()) {
            std::cerr << "CLI response parsing error: "
                << parsed.status().codeString()
                << ' '
                << parsed.status().message()
                << '\n'
            ;

            return EXIT_FAILURE;
        }

        const AsterKV::Client::ProtocolResponse &response = parsed.value();
        const std::string rendered = AsterKV::Client::renderPrettyResponse(response);

        if (response.kind == AsterKV::Client::ProtocolResponseKind::Error) {
            std::cerr << rendered;

            return EXIT_FAILURE;
        }

        std::cout << rendered;

        return EXIT_SUCCESS;
    }

    int runLocalMode(int argc, char **argv) {
        if (argc < 3) {
            std::cerr << "--local requires <command>\n";

            return EXIT_FAILURE;
        }

        AsterKV::Storage::InMemoryStorage storage;
        AsterKV::Pipeline::LocalPipeline pipeline {storage};

        const std::string commandLine = joinArguments(argc, argv, 2);
        const std::string response = pipeline.processLine(commandLine);

        return printUserResponse(response);
    }

    int runConnectSingleCommandMode(const AsterKV::Network::TcpEndpoint &endpoint, std::string_view cmd) {
        AsterKV::Network::TcpLineClient client {endpoint};

        auto response = client.sendCommandLine(cmd);
        if (response.isError()) {
            std::cerr << "TCP client error: " << response.status().codeString() << ' ' << response.status().message() << '\n';

            return EXIT_FAILURE;
        }

        return printUserResponse(response.value());
    }

    int runConnectReplMode(const AsterKV::Network::TcpEndpoint &endpoint) {
        AsterKV::Network::TcpLineClient client {endpoint};
        bool hadFailure = false;

        std::cout << "AsterKV TCP REPL connected to " << AsterKV::Network::tcpEndpointToString(endpoint) << '\n'
            << "Type 'exit' or 'quit' to leave.\n"
        ;

        std::string cmdLine;

        while (true) {
            std::cout << "asterkv > " << std::flush;
            if (!std::getline(std::cin, cmdLine)) {
                std::cout << '\n';
                break;
            }

            const std::string_view trimmed = trim(cmdLine);
            if (trimmed.empty()) {
                continue;
            }

            if (isExitCommand(trimmed)) {
                break;
            }

            auto response = client.sendCommandLine(trimmed);
            if (response.isError()) {
                hadFailure = true;

                std::cerr << "TCP client error: " << response.status().codeString() << ' ' << response.status().message() << '\n';
                continue;
            }

            if (printUserResponse(response.value()) != EXIT_SUCCESS) {
                hadFailure = true;
            }
        }

        return hadFailure ? EXIT_FAILURE : EXIT_SUCCESS;
    }


    int runConnectMode(int argc, char **argv) {
        if (argc < 3) {
            std::cerr << "--connect requires <host:port>\n";

            return EXIT_FAILURE;
        }

        auto endpoint = AsterKV::Network::parseTcpEndpoint(argv[2]);
        if (endpoint.isError()) {
            std::cerr << "Invalid connect address: " << endpoint.status().message() << '\n';

            return EXIT_FAILURE;
        }

        if (argc == 3) {
            return runConnectReplMode(endpoint.value());
        }

        const std::string cmdLine = joinArguments(argc, argv, 3);

        return runConnectSingleCommandMode(endpoint.value(), cmdLine);
    }
}

int main(int argc, char **argv) {
    const std::string_view execName = argc > 0 ? argv[0] : "astercli";

    if (argc == 1) {
        printUsage(execName);

        return EXIT_SUCCESS;
    }

    const std::string_view argument = argv[1];

    if (argument == "--version") {
        std::cout << AsterKV::Core::projectName() << " CLI " << AsterKV::Core::versionString() << '\n';

        return EXIT_SUCCESS;
    }

    if (argument == "--help") {
        printUsage(execName);

        return EXIT_SUCCESS;
    }

    if (argument == "--local") {
        return runLocalMode(argc, argv);
    }

    if (argument == "--connect") {
        return runConnectMode(argc, argv);
    }

    std::cerr << "Unknown argument: " << argument << std::endl;
    printUsage(execName);

    return EXIT_FAILURE;
}
