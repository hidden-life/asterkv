#include <asterkv/execution/command_response.h>

#include <utility>

namespace AsterKV::Execution {
    std::string_view commandResponseTypeToString(CommandResponseType type) noexcept {
        switch (type) {
            case CommandResponseType::SimpleString:
                return "simple_string";
            case CommandResponseType::BulkString:
                return "bulk_string";
            case CommandResponseType::Integer:
                return "integer";
        }

        return "unknown";
    }

    CommandResponse CommandResponse::simpleString(std::string value) {
        return CommandResponse(CommandResponseType::SimpleString, std::move(value), 0);
    }

    CommandResponse CommandResponse::bulkString(std::string value) {
        return CommandResponse(CommandResponseType::BulkString, std::move(value), 0);
    }

    CommandResponse CommandResponse::integer(std::int64_t value) {
        return CommandResponse(CommandResponseType::Integer, "", value);
    }

    CommandResponseType CommandResponse::type() const noexcept {
        return type_;
    }

    const std::string & CommandResponse::value() const noexcept {
        return value_;
    }

    std::int64_t CommandResponse::integerValue() const noexcept {
        return integerValue_;
    }

    CommandResponse::CommandResponse(CommandResponseType type, std::string value, std::int64_t integerValue) :
        type_(type), value_(std::move(value)), integerValue_(integerValue) {
    }
}
