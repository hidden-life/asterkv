#ifndef ASTERKV_COMMAND_COMMAND_H
#define ASTERKV_COMMAND_COMMAND_H

#include <asterkv/core/result.h>
#include <asterkv/core/status.h>

#include <vector>
#include <string>
#include <string_view>
#include <cstddef>

namespace AsterKV::Command {
    enum class CommandType {
        Ping,
        Set,
        Get,
        Del,
        Exists,
    };

    struct CommandRequest final {
        CommandType type;
        std::vector<std::string> arguments;
    };

    [[nodiscard]] std::string_view commandTypeToString(CommandType type) noexcept;
    [[nodiscard]] std::size_t expectedArgumentCount(CommandType type) noexcept;
    [[nodiscard]] Core::Result<CommandType> commandTypeFromString(std::string_view commandName);
    [[nodiscard]] Core::Status validateCommandRequest(const CommandRequest &request);
}

#endif //ASTERKV_COMMAND_COMMAND_H
