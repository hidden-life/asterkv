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
            << "       " << execName << " --local\n"
            << "       " << execName << " --local <command>\n\n"
            << "AsterKV server daemon.\n\n"
            << "Options:\n"
            << "    --version       Print version information.\n"
            << "    --help          Print this help message.\n"
            << "    --local         Run local stdin command mode without TCP networking\n\n"
            << "Examples:\n"
            << "  " << execName << " --local PING\n"
            << "  " << execName << " --local \"SET username alex\"\n"
            << "  printf 'PING'\\n | " << execName << " --local\n"
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

    std::cerr << "Unknown argument: " << argument << std::endl;
    printUsage(execName);

    return EXIT_FAILURE;
}
