#include <asterkv/command/command.h>
#include <asterkv/execution/command_dispatcher.h>
#include <asterkv/execution/command_response.h>
#include <asterkv/storage/in_memory_storage.h>

#include <cstdint>
#include <string>
#include <vector>

namespace {

[[nodiscard]] bool expectTrue(bool value) {
    return value;
}

[[nodiscard]] bool expectEqual(AsterKV::Execution::CommandResponseType left,
                               AsterKV::Execution::CommandResponseType right) {
    return left == right;
}

[[nodiscard]] bool expectEqual(AsterKV::Core::ErrorCode left,
                               AsterKV::Core::ErrorCode right) {
    return left == right;
}

[[nodiscard]] bool expectEqual(std::string_view left, std::string_view right) {
    return left == right;
}

[[nodiscard]] bool expectEqual(std::int64_t left, std::int64_t right) {
    return left == right;
}

[[nodiscard]] AsterKV::Command::CommandRequest makeRequest(
    AsterKV::Command::CommandType type,
    std::vector<std::string> arguments = {}) {
    return AsterKV::Command::CommandRequest{
        .type = type,
        .arguments = std::move(arguments),
    };
}

[[nodiscard]] bool testDispatchesPing() {
    AsterKV::Storage::InMemoryStorage storage;
    AsterKV::Execution::CommandDispatcher dispatcher{storage};

    const auto result = dispatcher.dispatch(
        makeRequest(AsterKV::Command::CommandType::Ping));

    if (!result.isOk()) {
        return false;
    }

    const AsterKV::Execution::CommandResponse& response = result.value();

    return expectEqual(response.type(), AsterKV::Execution::CommandResponseType::SimpleString) &&
           expectEqual(response.value(), "PONG");
}

[[nodiscard]] bool testDispatchesSetAndGet() {
    AsterKV::Storage::InMemoryStorage storage;
    AsterKV::Execution::CommandDispatcher dispatcher{storage};

    const auto setResult = dispatcher.dispatch(
        makeRequest(AsterKV::Command::CommandType::Set, {"username", "jackson"}));

    if (!setResult.isOk()) {
        return false;
    }

    const auto getResult = dispatcher.dispatch(
        makeRequest(AsterKV::Command::CommandType::Get, {"username"}));

    if (!getResult.isOk()) {
        return false;
    }

    const AsterKV::Execution::CommandResponse& setResponse = setResult.value();
    const AsterKV::Execution::CommandResponse& getResponse = getResult.value();

    return expectEqual(setResponse.type(), AsterKV::Execution::CommandResponseType::SimpleString) &&
           expectEqual(setResponse.value(), "OK") &&
           expectEqual(getResponse.type(), AsterKV::Execution::CommandResponseType::BulkString) &&
           expectEqual(getResponse.value(), "jackson");
}

[[nodiscard]] bool testGetMissingKeyReturnsNotFound() {
    AsterKV::Storage::InMemoryStorage storage;
    AsterKV::Execution::CommandDispatcher dispatcher{storage};

    const auto result = dispatcher.dispatch(
        makeRequest(AsterKV::Command::CommandType::Get, {"missing"}));

    if (result.isOk()) {
        return false;
    }

    return expectEqual(result.status().code(), AsterKV::Core::ErrorCode::NotFound);
}

[[nodiscard]] bool testDeleteExistingKeyReturnsOne() {
    AsterKV::Storage::InMemoryStorage storage;
    AsterKV::Execution::CommandDispatcher dispatcher{storage};

    const auto setResult = dispatcher.dispatch(
        makeRequest(AsterKV::Command::CommandType::Set, {"key", "value"}));

    if (!setResult.isOk()) {
        return false;
    }

    const auto delResult = dispatcher.dispatch(
        makeRequest(AsterKV::Command::CommandType::Del, {"key"}));

    if (!delResult.isOk()) {
        return false;
    }

    const AsterKV::Execution::CommandResponse& response = delResult.value();

    return expectEqual(response.type(), AsterKV::Execution::CommandResponseType::Integer) &&
           expectEqual(response.integerValue(), std::int64_t{1});
}

[[nodiscard]] bool testDeleteMissingKeyReturnsZero() {
    AsterKV::Storage::InMemoryStorage storage;
    AsterKV::Execution::CommandDispatcher dispatcher{storage};

    const auto result = dispatcher.dispatch(
        makeRequest(AsterKV::Command::CommandType::Del, {"missing"}));

    if (!result.isOk()) {
        return false;
    }

    const AsterKV::Execution::CommandResponse& response = result.value();

    return expectEqual(response.type(), AsterKV::Execution::CommandResponseType::Integer) &&
           expectEqual(response.integerValue(), std::int64_t{0});
}

[[nodiscard]] bool testExistsReturnsOneForExistingKey() {
    AsterKV::Storage::InMemoryStorage storage;
    AsterKV::Execution::CommandDispatcher dispatcher{storage};

    const auto setResult = dispatcher.dispatch(
        makeRequest(AsterKV::Command::CommandType::Set, {"key", "value"}));

    if (!setResult.isOk()) {
        return false;
    }

    const auto existsResult = dispatcher.dispatch(
        makeRequest(AsterKV::Command::CommandType::Exists, {"key"}));

    if (!existsResult.isOk()) {
        return false;
    }

    return expectEqual(existsResult.value().type(), AsterKV::Execution::CommandResponseType::Integer) &&
           expectEqual(existsResult.value().integerValue(), std::int64_t{1});
}

[[nodiscard]] bool testExistsReturnsZeroForMissingKey() {
    AsterKV::Storage::InMemoryStorage storage;
    AsterKV::Execution::CommandDispatcher dispatcher{storage};

    const auto result = dispatcher.dispatch(
        makeRequest(AsterKV::Command::CommandType::Exists, {"missing"}));

    if (!result.isOk()) {
        return false;
    }

    return expectEqual(result.value().type(), AsterKV::Execution::CommandResponseType::Integer) &&
           expectEqual(result.value().integerValue(), std::int64_t{0});
}

[[nodiscard]] bool testRejectsInvalidCommandRequest() {
    AsterKV::Storage::InMemoryStorage storage;
    AsterKV::Execution::CommandDispatcher dispatcher{storage};

    const auto result = dispatcher.dispatch(
        makeRequest(AsterKV::Command::CommandType::Get));

    if (result.isOk()) {
        return false;
    }

    return expectEqual(result.status().code(), AsterKV::Core::ErrorCode::InvalidArgument);
}

} // namespace

int main() {
    if (!testDispatchesPing()) {
        return 1;
    }

    if (!testDispatchesSetAndGet()) {
        return 1;
    }

    if (!testGetMissingKeyReturnsNotFound()) {
        return 1;
    }

    if (!testDeleteExistingKeyReturnsOne()) {
        return 1;
    }

    if (!testDeleteMissingKeyReturnsZero()) {
        return 1;
    }

    if (!testExistsReturnsOneForExistingKey()) {
        return 1;
    }

    if (!testExistsReturnsZeroForMissingKey()) {
        return 1;
    }

    if (!testRejectsInvalidCommandRequest()) {
        return 1;
    }

    return 0;
}
