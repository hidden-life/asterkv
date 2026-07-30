#include <asterkv/client/response_renderer.h>

#include <string>

namespace {

[[nodiscard]] bool testParsesSimpleString() {
    auto response = AsterKV::Client::parseProtocolResponse("+PONG\n");

    return response.isOk() &&
           response.value().kind == AsterKV::Client::ProtocolResponseKind::SimpleString &&
           response.value().value == "PONG";
}

[[nodiscard]] bool testParsesError() {
    auto response = AsterKV::Client::parseProtocolResponse(
        "-ERR not_found key not found\n");

    return response.isOk() &&
           response.value().kind == AsterKV::Client::ProtocolResponseKind::Error &&
           response.value().value == "ERR not_found key not found";
}

[[nodiscard]] bool testParsesInteger() {
    auto response = AsterKV::Client::parseProtocolResponse(":1\n");

    return response.isOk() &&
           response.value().kind == AsterKV::Client::ProtocolResponseKind::Integer &&
           response.value().value == "1";
}

[[nodiscard]] bool testParsesNegativeInteger() {
    auto response = AsterKV::Client::parseProtocolResponse(":-1\n");

    return response.isOk() &&
           response.value().kind == AsterKV::Client::ProtocolResponseKind::Integer &&
           response.value().value == "-1";
}

[[nodiscard]] bool testParsesBulkString() {
    auto response = AsterKV::Client::parseProtocolResponse("$7\njackson\n");

    return response.isOk() &&
           response.value().kind == AsterKV::Client::ProtocolResponseKind::BulkString &&
           response.value().value == "jackson";
}

[[nodiscard]] bool testParsesCrLfResponse() {
    auto response = AsterKV::Client::parseProtocolResponse("$7\r\njackson\r\n");

    return response.isOk() &&
           response.value().kind == AsterKV::Client::ProtocolResponseKind::BulkString &&
           response.value().value == "jackson";
}

[[nodiscard]] bool testRendersPrettySimpleString() {
    AsterKV::Client::ProtocolResponse response{
        .kind = AsterKV::Client::ProtocolResponseKind::SimpleString,
        .value = "OK",
    };

    return AsterKV::Client::renderPrettyResponse(response) == "OK\n";
}

[[nodiscard]] bool testRendersPrettyBulkString() {
    AsterKV::Client::ProtocolResponse response{
        .kind = AsterKV::Client::ProtocolResponseKind::BulkString,
        .value = "jackson",
    };

    return AsterKV::Client::renderPrettyResponse(response) == "jackson\n";
}

[[nodiscard]] bool testRendersPrettyError() {
    AsterKV::Client::ProtocolResponse response{
        .kind = AsterKV::Client::ProtocolResponseKind::Error,
        .value = "ERR not_found key not found",
    };

    return AsterKV::Client::renderPrettyResponse(response) ==
           "error: not_found key not found\n";
}

[[nodiscard]] bool testRendersPrettyResponseText() {
    auto rendered = AsterKV::Client::renderPrettyResponseText("$7\njackson\n");

    return rendered.isOk() &&
           rendered.value() == "jackson\n";
}

[[nodiscard]] bool testRejectsEmptyResponse() {
    auto response = AsterKV::Client::parseProtocolResponse("");

    return response.isError();
}

[[nodiscard]] bool testRejectsUnknownPrefix() {
    auto response = AsterKV::Client::parseProtocolResponse("?hello\n");

    return response.isError();
}

[[nodiscard]] bool testRejectsInvalidInteger() {
    auto response = AsterKV::Client::parseProtocolResponse(":abc\n");

    return response.isError();
}

[[nodiscard]] bool testRejectsInvalidBulkLength() {
    auto response = AsterKV::Client::parseProtocolResponse("$abc\nvalue\n");

    return response.isError();
}

[[nodiscard]] bool testRejectsTruncatedBulkString() {
    auto response = AsterKV::Client::parseProtocolResponse("$7\njack\n");

    return response.isError();
}

} // namespace

int main() {
    if (!testParsesSimpleString()) {
        return 1;
    }

    if (!testParsesError()) {
        return 1;
    }

    if (!testParsesInteger()) {
        return 1;
    }

    if (!testParsesNegativeInteger()) {
        return 1;
    }

    if (!testParsesBulkString()) {
        return 1;
    }

    if (!testParsesCrLfResponse()) {
        return 1;
    }

    if (!testRendersPrettySimpleString()) {
        return 1;
    }

    if (!testRendersPrettyBulkString()) {
        return 1;
    }

    if (!testRendersPrettyError()) {
        return 1;
    }

    if (!testRendersPrettyResponseText()) {
        return 1;
    }

    if (!testRejectsEmptyResponse()) {
        return 1;
    }

    if (!testRejectsUnknownPrefix()) {
        return 1;
    }

    if (!testRejectsInvalidInteger()) {
        return 1;
    }

    if (!testRejectsInvalidBulkLength()) {
        return 1;
    }

    if (!testRejectsTruncatedBulkString()) {
        return 1;
    }

    return 0;
}
