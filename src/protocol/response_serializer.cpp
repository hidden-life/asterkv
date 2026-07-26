#include <asterkv/protocol/response_serializer.h>

namespace AsterKV::Protocol {
    namespace {
        constexpr std::string_view crlf = "\r\n";

        [[nodiscard]] std::string serializeSimpleString(std::string_view value) {
            std::string result;

            result.reserve(1 + value.size() + crlf.size());
            result.push_back('+');
            result.append(value);
            result.append(crlf);

            return result;
        }

        [[nodiscard]] std::string serializeBulkString(std::string_view value) {
            const std::string size = std::to_string(value.size());
            std::string result;

            result.reserve(1 + size.size() + crlf.size() + value.size() + crlf.size());
            result.push_back('$');
            result.append(size);
            result.append(crlf);
            result.append(value);
            result.append(crlf);

            return result;
        }

        [[nodiscard]] std::string serializeInteger(std::int64_t value) {
            const std::string integer = std::to_string(value);
            std::string result;

            result.reserve(1 + integer.size() + crlf.size());
            result.push_back(':');
            result.append(integer);
            result.append(crlf);

            return result;
        }
    }

    std::string serializeCommandResponse(const Execution::CommandResponse &response) {
        switch (response.type()) {
            case Execution::CommandResponseType::SimpleString:
                return serializeSimpleString(response.value());
            case Execution::CommandResponseType::BulkString:
                return serializeBulkString(response.value());
            case Execution::CommandResponseType::Integer:
                return serializeInteger(response.integerValue());
        }

        return "-ERR internal unsupported response type\r\n";
    }

    std::string serializeStatus(const Core::Status &status) {
        if (status.isOk()) {
            return serializeSimpleString("OK");
        }

        std::string result;

        result.reserve(5 + status.codeString().size() + 1 + status.message().size() + crlf.size());
        result.append("-ERR ");
        result.append(status.codeString());

        if (!status.message().empty()) {
            result.push_back(' ');
            result.append(status.message());
        }

        result.append(crlf);

        return result;
    }

    std::string serializeExecutionResult(const Core::Result<Execution::CommandResponse> &result) {
        if (result.isOk()) {
            return serializeCommandResponse(result.value());
        }

        return serializeStatus(result.status());
    }
}
