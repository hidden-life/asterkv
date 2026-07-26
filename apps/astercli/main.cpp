#include <asterkv/core/version.h>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <string_view>

#include "asterkv/pipeline/local_pipeline.h"
#include "asterkv/storage/in_memory_storage.h"

namespace {
    void printUsage(std::string_view execName) {
        std::cout
            << "Usage: " << execName << " [--version] [--help]\n"
            << "       " << execName << " local\n"
            << "       " << execName << " local <command>\n\n"
            << "AsterKV user CLI.\n\n"
            << "Options:\n"
            << "    --version       Print version information.\n"
            << "    --help          Print this help message.\n"
            << "Commands:\n"
            << "    local           Start local in-memory REPL\n"
            << "    local <command> Execute one command locally\n\n"
            << "Examples:\n"
            << "    " << execName << " local\n"
            << "    " << execName << " local PING\n"
            << "    " << execName << " local \"SET username alex\"\n"
        ;
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

    int runLocalRepl() {
        AsterKV::Storage::InMemoryStorage storage;
        AsterKV::Pipeline::LocalPipeline pipeline {storage};

        std::cout << "AsterKV local mode " << AsterKV::Core::versionString() << '\n';
        std::cout << "Type commands or 'quit' to exit.\n";

        std::string line;

        while (true) {
            std::cout << "> ";

            if (!std::getline(std::cin, line)) {
                std::cout << '\n';
                return EXIT_SUCCESS;
            }

            if (line == "quit" || line == "exit") {
                return EXIT_SUCCESS;
            }

            if (line.empty()) {
                continue;
            }

            const std::string response = pipeline.processLine(line);

            printProtocolResponse(response);
        }
    }

    int runLocalMode(int argc, char **argv) {
        if (argc == 2) {
            return runLocalRepl();
        }

        return runLocalSingleCommand(argc, argv);
    }
}

int main(const int argc, char **argv) {
    const std::string_view execName = argc > 0 ? argv[0] : "astercli";

    if (argc == 1) {
        printUsage(execName);

        return EXIT_SUCCESS;
    }

    const std::string_view argument = argv[1];

    if (argument == "--version") {
        std::cout << AsterKV::Core::projectName() << " cli " << AsterKV::Core::versionString() << std::endl;

        return EXIT_SUCCESS;
    }

    if (argument == "--help") {
        printUsage(execName);

        return EXIT_SUCCESS;
    }

    if (argument == "local") {
        return runLocalMode(argc, argv);
    }

    std::cerr << "Unknown argument: " << argument << std::endl;
    printUsage(execName);

    return EXIT_FAILURE;
}
