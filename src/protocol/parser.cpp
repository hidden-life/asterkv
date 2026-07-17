#include <asterkv/protocol/parser.h>

namespace AsterKV::Protocol {
    namespace {
        [[nodiscard]] bool isAsciiWhitespace(char value) noexcept {
            const auto character = static_cast<unsigned char>(value);

            return std::isspace(character) != 0;
        }

        [[nodiscard]] bool isBlank(std::string_view line) noexcept {
            for (const char c: line) {
                if (!isAsciiWhitespace(c))
                    return false;
            }

            return true;
        }
    }

    std::vector<std::string> tokenizeCommandLine(std::string_view line) {
        std::vector<std::string> tokens;
        std::size_t idx = 0;

        while (idx < line.size()) {
            while (idx < line.size() && isAsciiWhitespace(line[idx])) {
                ++idx;
            }

            if (idx >= line.size()) {
                break;
            }

            const std::size_t tokenStart = idx;

            while (idx < line.size() && !isAsciiWhitespace(line[idx])) {
                ++idx;
            }

            tokens.emplace_back(line.substr(tokenStart, idx - tokenStart));
        }

        return tokens;
    }

    Core::Result<Command::CommandRequest> parseCommandLine(std::string_view line) {
        if (line.empty() || isBlank(line)) {
            return Core::Result<Command::CommandRequest>::failure(
                Core::Status::invalidArgument("command line must not be empty")
            );
        }

        std::vector<std::string> tokens = tokenizeCommandLine(line);

        if (tokens.empty()) {
            return Core::Result<Command::CommandRequest>::failure(
                Core::Status::invalidArgument("command line must not be empty")
            );
        }

        Core::Result<Command::CommandType> cmdType = Command::commandTypeFromString(tokens.front());
        if (cmdType.isError()) {
            return Core::Result<Command::CommandRequest>::failure(cmdType.status());
        }

        std::vector<std::string> arguments;
        arguments.reserve(tokens.size() - 1);

        for (std::size_t idx = 1; idx < tokens.size(); ++idx) {
            arguments.push_back(std::move(tokens[idx]));
        }

        Command::CommandRequest request {
            .type = cmdType.value(),
            .arguments = std::move(arguments),
        };

        Core::Status validationStatus = Command::validateCommandRequest(request);
        if (!validationStatus.isOk()) {
            return Core::Result<Command::CommandRequest>::failure(std::move(validationStatus));
        }

        return Core::Result<Command::CommandRequest>::success(std::move(request));
    }
}
