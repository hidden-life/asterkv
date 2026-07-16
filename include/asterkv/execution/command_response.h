#ifndef ASTERKV_EXECUTION_COMMAND_RESPONSE_H
#define ASTERKV_EXECUTION_COMMAND_RESPONSE_H

#include <cstdint>
#include <string>
#include <string_view>

namespace AsterKV::Execution {
    enum class CommandResponseType {
        SimpleString,
        BulkString,
        Integer,
    };

    [[nodiscard]] std::string_view commandResponseTypeToString(CommandResponseType type) noexcept;

    class CommandResponse final {
    public:
        [[nodiscard]] static CommandResponse simpleString(std::string value);
        [[nodiscard]] static CommandResponse bulkString(std::string value);
        [[nodiscard]] static CommandResponse integer(std::int64_t value);

        [[nodiscard]] CommandResponseType type() const noexcept;
        [[nodiscard]] const std::string &value() const noexcept;
        [[nodiscard]] std::int64_t integerValue() const noexcept;

    private:
        CommandResponse(CommandResponseType type, std::string value, std::int64_t integerValue);

        CommandResponseType type_;
        std::string value_;
        std::int64_t integerValue_;
    };
}

#endif //ASTERKV_EXECUTION_COMMAND_RESPONSE_H
