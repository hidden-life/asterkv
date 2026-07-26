#include <asterkv/storage/in_memory_storage.h>

#include <string>

namespace {

[[nodiscard]] bool expectTrue(bool value) {
    return value;
}

[[nodiscard]] bool expectFalse(bool value) {
    return !value;
}

[[nodiscard]] bool expectEqual(AsterKV::Core::ErrorCode left,
                               AsterKV::Core::ErrorCode right) {
    return left == right;
}

[[nodiscard]] bool expectEqual(std::string_view left, std::string_view right) {
    return left == right;
}

[[nodiscard]] bool testStoresAndReadsValue() {
    AsterKV::Storage::InMemoryStorage storage;

    const AsterKV::Core::Status setStatus = storage.set("username", "jackson");

    if (!setStatus.isOk()) {
        return false;
    }

    const auto result = storage.get("username");

    return result.isOk() &&
           expectEqual(result.value(), "jackson") &&
           storage.size() == 1;
}

[[nodiscard]] bool testOverwritesExistingValue() {
    AsterKV::Storage::InMemoryStorage storage;

    if (!storage.set("username", "jackson").isOk()) {
        return false;
    }

    if (!storage.set("username", "aster").isOk()) {
        return false;
    }

    const auto result = storage.get("username");

    return result.isOk() &&
           expectEqual(result.value(), "aster") &&
           storage.size() == 1;
}

[[nodiscard]] bool testReturnsNotFoundForMissingKey() {
    AsterKV::Storage::InMemoryStorage storage;

    const auto result = storage.get("missing");

    if (result.isOk()) {
        return false;
    }

    return expectEqual(result.status().code(), AsterKV::Core::ErrorCode::NotFound);
}

[[nodiscard]] bool testDeletesExistingKey() {
    AsterKV::Storage::InMemoryStorage storage;

    if (!storage.set("key", "value").isOk()) {
        return false;
    }

    if (!storage.remove("key").isOk()) {
        return false;
    }

    const auto result = storage.get("key");

    return result.isError() &&
           expectEqual(result.status().code(), AsterKV::Core::ErrorCode::NotFound) &&
           storage.size() == 0;
}

[[nodiscard]] bool testDeleteMissingKeyReturnsNotFound() {
    AsterKV::Storage::InMemoryStorage storage;

    const AsterKV::Core::Status status = storage.remove("missing");

    return !status.isOk() &&
           expectEqual(status.code(), AsterKV::Core::ErrorCode::NotFound);
}

[[nodiscard]] bool testExistsReturnsTrueForExistingKey() {
    AsterKV::Storage::InMemoryStorage storage;

    if (!storage.set("key", "value").isOk()) {
        return false;
    }

    const auto result = storage.exists("key");

    return result.isOk() &&
           expectTrue(result.value());
}

[[nodiscard]] bool testExistsReturnsFalseForMissingKey() {
    AsterKV::Storage::InMemoryStorage storage;

    const auto result = storage.exists("missing");

    return result.isOk() &&
           expectFalse(result.value());
}

[[nodiscard]] bool testRejectsEmptyKey() {
    AsterKV::Storage::InMemoryStorage storage;

    const AsterKV::Core::Status setStatus = storage.set("", "value");
    const auto getResult = storage.get("");
    const AsterKV::Core::Status removeStatus = storage.remove("");
    const auto existsResult = storage.exists("");

    return !setStatus.isOk() &&
           expectEqual(setStatus.code(), AsterKV::Core::ErrorCode::InvalidArgument) &&
           getResult.isError() &&
           expectEqual(getResult.status().code(), AsterKV::Core::ErrorCode::InvalidArgument) &&
           !removeStatus.isOk() &&
           expectEqual(removeStatus.code(), AsterKV::Core::ErrorCode::InvalidArgument) &&
           existsResult.isError() &&
           expectEqual(existsResult.status().code(), AsterKV::Core::ErrorCode::InvalidArgument);
}

[[nodiscard]] bool testClearRemovesAllKeys() {
    AsterKV::Storage::InMemoryStorage storage;

    if (!storage.set("key1", "value1").isOk()) {
        return false;
    }

    if (!storage.set("key2", "value2").isOk()) {
        return false;
    }

    storage.clear();

    return storage.size() == 0;
}

} // namespace

int main() {
    if (!testStoresAndReadsValue()) {
        return 1;
    }

    if (!testOverwritesExistingValue()) {
        return 1;
    }

    if (!testReturnsNotFoundForMissingKey()) {
        return 1;
    }

    if (!testDeletesExistingKey()) {
        return 1;
    }

    if (!testDeleteMissingKeyReturnsNotFound()) {
        return 1;
    }

    if (!testExistsReturnsTrueForExistingKey()) {
        return 1;
    }

    if (!testExistsReturnsFalseForMissingKey()) {
        return 1;
    }

    if (!testRejectsEmptyKey()) {
        return 1;
    }

    if (!testClearRemovesAllKeys()) {
        return 1;
    }

    return 0;
}
