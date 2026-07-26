#include <asterkv/core/version.h>
#include <cstdlib>

#include <iostream>
#include <string_view>

namespace {
    void printUsage(std::string_view execName) {
        std::cout
            << "Usage: " << execName << " [--version] [--help]" << std::endl << std::endl
            << "AsterKV administrative control tool." << std::endl << std::endl
            << "Options:" << std::endl
            << "    --version    Print version information." << std::endl
            << "    --help       Print this help message." << std::endl
        ;
    }
}

int main(int argc, char **argv) {
    const std::string_view execName = argc > 0 ? argv[0] : "asterctl";

    if (argc == 1) {
        printUsage(execName);

        return EXIT_SUCCESS;
    }

    const std::string_view argument = argv[1];

    if (argument == "--version") {
        std::cout << AsterKV::Core::projectName() << " control " << AsterKV::Core::versionString() << std::endl;

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
