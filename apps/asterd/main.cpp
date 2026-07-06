#include "asterkv/core/version.h"

#include <iostream>
#include <string_view>

namespace {
    void printUsage(std::string_view execName) {
        std::cout
            << "Usage: " << execName << " [--version] [--help]" << std::endl
            << "AsterKV server daemon." << std::endl << std::endl
            << "Options:" << std::endl
            << "    --version    Print version information." << std::endl
            << "    --help       Print this help message." << std::endl
        ;
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
        std::cout << AsterKV::Core::projectName() << " server " << AsterKV::Core::versionString() << std::endl;

        return EXIT_SUCCESS;
    }

    if (argument == "--help") {
        printUsage(execName);

        return EXIT_SUCCESS;
    }

    std::cerr << "Unknown argument: " << argument << std::endl;
    printUsage(execName);

    return EXIT_FAILURE;
}
