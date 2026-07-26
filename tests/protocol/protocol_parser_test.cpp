#include <asterkv/protocol/parser.h>

#include <string>
#include <vector>

namespace {

[[nodiscard]] bool expectTrue(bool value) {
    return value;
}

[[nodiscard]] bool expectEqual(AsterKV::Command::CommandType left,
                               AsterKV::Command::CommandType right) {
    return left == right;
}

[[nodiscard]] bool expectEqual(AsterKV::Core::ErrorCode left,
                               AsterKV::Core::ErrorCode right) {
    return left == right;
}

[[nodiscard]] bool expectEqual(std::string_view left, std::string_view right) {
    return left == right;
}

[[nodiscard]] bool testTokenizesWhitespaceSeparatedLine() {
    const std::vector<std::string> tokens =
        AsterKV::Protocol::tokenizeCommandLine("  SET   key   value  ");

    return tokens.size() == 3 &&
           expectEqual(tokens[0], "SET") &&
           expectEqual(tokens[1], "key") &&
           expectEqual(tokens[2], "value");
}

[[nodiscard]] bool testParsesPingCommand() {
    const auto result = AsterKV::Protocol::parseCommandLine("PING");

    if (!result.isOk()) {
        return false;
    }

    const AsterKV::Command::CommandRequest& request = result.value();

    return expectEqual(request.type, AsterKV::Command::CommandType::Ping) &&
           request.arguments.empty();
}

[[nodiscard]] bool testParsesSetCommand() {
    const auto result = AsterKV::Protocol::parseCommandLine("SET username jackson");

    if (!result.isOk()) {
        return false;
    }

    const AsterKV::Command::CommandRequest& request = result.value();

    return expectEqual(request.type, AsterKV::Command::CommandType::Set) &&
           request.arguments.size() == 2 &&
           expectEqual(request.arguments[0], "username") &&
           expectEqual(request.arguments[1], "jackson");
}

[[nodiscard]] bool testParsesCommandCaseInsensitively() {
    const auto result = AsterKV::Protocol::parseCommandLine("get username");

    if (!result.isOk()) {
        return false;
    }

    const AsterKV::Command::CommandRequest& request = result.value();

    return expectEqual(request.type, AsterKV::Command::CommandType::Get) &&
           request.arguments.size() == 1 &&
           expectEqual(request.arguments[0], "username");
}

[[nodiscard]] bool testRejectsEmptyLine() {
    const auto result = AsterKV::Protocol::parseCommandLine("   ");

    if (result.isOk()) {
        return false;
    }

    return expectEqual(result.status().code(), AsterKV::Core::ErrorCode::InvalidArgument);
}

[[nodiscard]] bool testRejectsUnknownCommand() {
    const auto result = AsterKV::Protocol::parseCommandLine("NOPE key");

    if (result.isOk()) {
        return false;
    }

    return expectEqual(result.status().code(), AsterKV::Core::ErrorCode::InvalidArgument);
}

[[nodiscard]] bool testRejectsInvalidArgumentCount() {
    const auto result = AsterKV::Protocol::parseCommandLine("GET");

    if (result.isOk()) {
        return false;
    }

    return expectEqual(result.status().code(), AsterKV::Core::ErrorCode::InvalidArgument);
}

} // namespace

int main() {
    if (!testTokenizesWhitespaceSeparatedLine()) {
        return 1;
    }

    if (!testParsesPingCommand()) {
        return 1;
    }

    if (!testParsesSetCommand()) {
        return 1;
    }

    if (!testParsesCommandCaseInsensitively()) {
        return 1;
    }

    if (!testRejectsEmptyLine()) {
        return 1;
    }

    if (!testRejectsUnknownCommand()) {
        return 1;
    }

    if (!testRejectsInvalidArgumentCount()) {
        return 1;
    }

    return 0;
}
