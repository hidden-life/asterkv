#include <asterkv/command/command.h>

namespace AsterKV::Command {
    namespace {
        [[nodiscard]] bool asciiEqualsIgnoreCase(std::string_view left, std::string_view right) noexcept {
            if (left.size() != right.size()) {
                return false;
            }

            for (std::size_t idx = 0; idx < left.size(); ++idx) {
                const auto leftChar = static_cast<unsigned char>(left[idx]);
                const auto rightChar = static_cast<unsigned char>(right[idx]);

                if (std::toupper(leftChar) != std::toupper(rightChar)) {
                    return false;
                }
            }

            return true;
        }
    }

    std::string_view commandTypeToString(CommandType type) noexcept {
        switch (type) {
            case CommandType::Ping:
                return "PING";
            case CommandType::Set:
                return "SET";
            case CommandType::Get:
                return "GET";
            case CommandType::Del:
                return "DEL";
            case CommandType::Exists:
                return "EXISTS";
        }

        return "UNKNOWN";
    }

    std::size_t expectedArgumentCount(CommandType type) noexcept {
        switch (type) {
            case CommandType::Ping:
                return 0;
            case CommandType::Set:
                return 2;
            case CommandType::Get:
                return 1;
            case CommandType::Del:
                return 1;
            case CommandType::Exists:
                return 1;
        }

        return 0;
    }

    Core::Result<CommandType> commandTypeFromString(std::string_view commandName) {
        if (commandName.empty()) {
            return Core::Result<CommandType>::failure(
                Core::Status::invalidArgument("command name must not be empty")
            );
        }

        if (asciiEqualsIgnoreCase(commandName, "PING")) {
            return Core::Result<CommandType>::success(CommandType::Ping);
        }

        if (asciiEqualsIgnoreCase(commandName, "SET")) {
            return Core::Result<CommandType>::success(CommandType::Set);
        }

        if (asciiEqualsIgnoreCase(commandName, "GET")) {
            return Core::Result<CommandType>::success(CommandType::Get);
        }

        if (asciiEqualsIgnoreCase(commandName, "DEL")) {
            return Core::Result<CommandType>::success(CommandType::Del);
        }

        if (asciiEqualsIgnoreCase(commandName, "EXISTS")) {
            return Core::Result<CommandType>::success(CommandType::Exists);
        }

        return Core::Result<CommandType>::failure(
            Core::Status::invalidArgument("unknown command")
        );
    }

    Core::Status validateCommandRequest(const CommandRequest &request) {
        const std::size_t expectedCount = expectedArgumentCount(request.type);
        if (request.arguments.size() != expectedCount) {
            return Core::Status::invalidArgument("invalid command argument count");
        }

        return Core::Status::ok();
    }
}
