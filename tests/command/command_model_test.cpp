#include <asterkv/command/command.h>
#include <asterkv/core/status.h>

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

    [[nodiscard]] bool testParsesKnownCommands() {
        const auto ping = AsterKV::Command::commandTypeFromString("PING");
        const auto set = AsterKV::Command::commandTypeFromString("set");
        const auto get = AsterKV::Command::commandTypeFromString("Get");

        return expectTrue(ping.isOk()) &&
               expectTrue(set.isOk()) &&
               expectTrue(get.isOk()) &&
               expectEqual(ping.value(), AsterKV::Command::CommandType::Ping) &&
               expectEqual(set.value(), AsterKV::Command::CommandType::Set) &&
               expectEqual(get.value(), AsterKV::Command::CommandType::Get);
    }

    [[nodiscard]] bool testRejectsUnknownCommand() {
        const auto result = AsterKV::Command::commandTypeFromString("NOPE");

        if (result.isOk()) {
            return false;
        }

        const AsterKV::Core::Status status = result.status();

        return expectEqual(status.code(), AsterKV::Core::ErrorCode::InvalidArgument);
    }

    [[nodiscard]] bool testExpectedArgumentCounts() {
        return AsterKV::Command::expectedArgumentCount(AsterKV::Command::CommandType::Ping) == 0 &&
               AsterKV::Command::expectedArgumentCount(AsterKV::Command::CommandType::Set) == 2 &&
               AsterKV::Command::expectedArgumentCount(AsterKV::Command::CommandType::Get) == 1 &&
               AsterKV::Command::expectedArgumentCount(AsterKV::Command::CommandType::Del) == 1 &&
               AsterKV::Command::expectedArgumentCount(AsterKV::Command::CommandType::Exists) == 1;
    }

    [[nodiscard]] bool testValidatesValidRequest() {
        const AsterKV::Command::CommandRequest request{
            .type = AsterKV::Command::CommandType::Set,
            .arguments = std::vector<std::string>{"key", "value"},
        };

        return AsterKV::Command::validateCommandRequest(request).isOk();
    }

    [[nodiscard]] bool testRejectsInvalidArgumentCount() {
        const AsterKV::Command::CommandRequest request{
            .type = AsterKV::Command::CommandType::Get,
            .arguments = {},
        };

        const AsterKV::Core::Status status = AsterKV::Command::validateCommandRequest(request);

        return !status.isOk() &&
               expectEqual(status.code(), AsterKV::Core::ErrorCode::InvalidArgument);
    }
}

int main() {
    if (!testParsesKnownCommands()) {
        return 1;
    }

    if (!testRejectsUnknownCommand()) {
        return 1;
    }

    if (!testExpectedArgumentCounts()) {
        return 1;
    }

    if (!testValidatesValidRequest()) {
        return 1;
    }

    if (!testRejectsInvalidArgumentCount()) {
        return 1;
    }

    return 0;
}
