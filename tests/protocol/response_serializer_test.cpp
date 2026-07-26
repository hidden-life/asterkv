#include <asterkv/core/result.h>
#include <asterkv/core/status.h>
#include <asterkv/execution/command_response.h>
#include <asterkv/protocol/response_serializer.h>

#include <cstdint>
#include <string>

namespace {

[[nodiscard]] bool expectEqual(std::string_view left, std::string_view right) {
    return left == right;
}

[[nodiscard]] bool testSerializesSimpleString() {
    const AsterKV::Execution::CommandResponse response =
        AsterKV::Execution::CommandResponse::simpleString("OK");

    return expectEqual(AsterKV::Protocol::serializeCommandResponse(response), "+OK\r\n");
}

[[nodiscard]] bool testSerializesBulkString() {
    const AsterKV::Execution::CommandResponse response =
        AsterKV::Execution::CommandResponse::bulkString("value");

    return expectEqual(AsterKV::Protocol::serializeCommandResponse(response), "$5\r\nvalue\r\n");
}

[[nodiscard]] bool testSerializesEmptyBulkString() {
    const AsterKV::Execution::CommandResponse response =
        AsterKV::Execution::CommandResponse::bulkString("");

    return expectEqual(AsterKV::Protocol::serializeCommandResponse(response), "$0\r\n\r\n");
}

[[nodiscard]] bool testSerializesInteger() {
    const AsterKV::Execution::CommandResponse response =
        AsterKV::Execution::CommandResponse::integer(std::int64_t{1});

    return expectEqual(AsterKV::Protocol::serializeCommandResponse(response), ":1\r\n");
}

[[nodiscard]] bool testSerializesOkStatus() {
    const AsterKV::Core::Status status = AsterKV::Core::Status::ok();

    return expectEqual(AsterKV::Protocol::serializeStatus(status), "+OK\r\n");
}

[[nodiscard]] bool testSerializesErrorStatusWithMessage() {
    const AsterKV::Core::Status status =
        AsterKV::Core::Status::notFound("key not found");

    return expectEqual(AsterKV::Protocol::serializeStatus(status),
                       "-ERR not_found key not found\r\n");
}

[[nodiscard]] bool testSerializesErrorStatusWithoutMessage() {
    const AsterKV::Core::Status status =
        AsterKV::Core::Status::internalError("");

    return expectEqual(AsterKV::Protocol::serializeStatus(status), "-ERR internal\r\n");
}

[[nodiscard]] bool testSerializesSuccessfulExecutionResult() {
    auto result = AsterKV::Core::Result<AsterKV::Execution::CommandResponse>::success(
        AsterKV::Execution::CommandResponse::simpleString("PONG"));

    return expectEqual(AsterKV::Protocol::serializeExecutionResult(result), "+PONG\r\n");
}

[[nodiscard]] bool testSerializesFailedExecutionResult() {
    auto result = AsterKV::Core::Result<AsterKV::Execution::CommandResponse>::failure(
        AsterKV::Core::Status::notFound("key not found"));

    return expectEqual(AsterKV::Protocol::serializeExecutionResult(result),
                       "-ERR not_found key not found\r\n");
}

} // namespace

int main() {
    if (!testSerializesSimpleString()) {
        return 1;
    }

    if (!testSerializesBulkString()) {
        return 1;
    }

    if (!testSerializesEmptyBulkString()) {
        return 1;
    }

    if (!testSerializesInteger()) {
        return 1;
    }

    if (!testSerializesOkStatus()) {
        return 1;
    }

    if (!testSerializesErrorStatusWithMessage()) {
        return 1;
    }

    if (!testSerializesErrorStatusWithoutMessage()) {
        return 1;
    }

    if (!testSerializesSuccessfulExecutionResult()) {
        return 1;
    }

    if (!testSerializesFailedExecutionResult()) {
        return 1;
    }

    return 0;
}
