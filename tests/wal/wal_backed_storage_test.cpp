#include <asterkv/wal/wal_backed_storage.h>

#include <asterkv/storage/in_memory_storage.h>
#include <asterkv/wal/wal_file_reader.h>

#include <chrono>
#include <filesystem>
#include <string>
#include <string_view>

namespace {

[[nodiscard]] std::filesystem::path makeTempWalPath(std::string_view suffix) {
    const auto now = std::chrono::steady_clock::now()
                         .time_since_epoch()
                         .count();

    std::string fileName = "asterkv_wal_backed_storage_test_";
    fileName.append(std::to_string(now));
    fileName.push_back('_');
    fileName.append(suffix);
    fileName.append(".wal");

    return std::filesystem::temp_directory_path() / fileName;
}

void removeIfExists(const std::filesystem::path& path) {
    std::error_code error;
    std::filesystem::remove(path, error);
}

[[nodiscard]] bool testSetWritesWalAndUpdatesStorage() {
    const std::filesystem::path path = makeTempWalPath("set");
    removeIfExists(path);

    AsterKV::Storage::InMemoryStorage storage;
    AsterKV::Wal::WalBackedStorage walStorage{storage, path.string()};

    const AsterKV::Core::Status status =
        walStorage.set("username", "alex");

    const auto records = AsterKV::Wal::readRecordsFromFile(path.string());
    const auto value = storage.get("username");

    removeIfExists(path);

    return status.isOk() &&
           records.isOk() &&
           records.value().size() == 1 &&
           records.value()[0].sequenceNumber == 1 &&
           records.value()[0].type == AsterKV::Wal::WalRecordType::Set &&
           records.value()[0].key == "username" &&
           records.value()[0].value == "alex" &&
           value.isOk() &&
           value.value() == "alex";
}

[[nodiscard]] bool testDelWritesWalAndUpdatesStorage() {
    const std::filesystem::path path = makeTempWalPath("del");
    removeIfExists(path);

    AsterKV::Storage::InMemoryStorage storage;
    AsterKV::Wal::WalBackedStorage walStorage{storage, path.string()};

    const AsterKV::Core::Status setStatus =
        walStorage.set("username", "alex");

    const AsterKV::Core::Status delStatus =
        walStorage.del("username");

    const auto records = AsterKV::Wal::readRecordsFromFile(path.string());
    const auto value = storage.get("username");

    removeIfExists(path);

    return setStatus.isOk() &&
           delStatus.isOk() &&
           records.isOk() &&
           records.value().size() == 2 &&
           records.value()[0].type == AsterKV::Wal::WalRecordType::Set &&
           records.value()[1].type == AsterKV::Wal::WalRecordType::Del &&
           !value.isOk();
}

[[nodiscard]] bool testRecoverRestoresStorageState() {
    const std::filesystem::path path = makeTempWalPath("recover");
    removeIfExists(path);

    {
        AsterKV::Storage::InMemoryStorage storage;
        AsterKV::Wal::WalBackedStorage walStorage{storage, path.string()};

        if (!walStorage.set("username", "alex").isOk()) {
            removeIfExists(path);
            return false;
        }

        if (!walStorage.set("role", "admin").isOk()) {
            removeIfExists(path);
            return false;
        }

        if (!walStorage.del("username").isOk()) {
            removeIfExists(path);
            return false;
        }
    }

    AsterKV::Storage::InMemoryStorage recoveredStorage;
    AsterKV::Wal::WalBackedStorage recoveredWalStorage{
        recoveredStorage,
        path.string(),
    };

    const AsterKV::Core::Status recoverStatus =
        recoveredWalStorage.recover();

    const auto username = recoveredStorage.get("username");
    const auto role = recoveredStorage.get("role");

    removeIfExists(path);

    return recoverStatus.isOk() &&
           !username.isOk() &&
           role.isOk() &&
           role.value() == "admin" &&
           recoveredWalStorage.nextSequenceNumber() == 4;
}

[[nodiscard]] bool testRejectsEmptyWalPathOnSet() {
    AsterKV::Storage::InMemoryStorage storage;
    AsterKV::Wal::WalBackedStorage walStorage{storage, ""};

    const AsterKV::Core::Status status =
        walStorage.set("username", "alex");

    return !status.isOk();
}

    [[nodiscard]] bool testWalBackedStorageAppendsMultipleMutations() {
    const std::filesystem::path path = makeTempWalPath("multi_mutation");
    removeIfExists(path);

    AsterKV::Storage::InMemoryStorage storage;
    AsterKV::Wal::WalBackedStorage walStorage{storage, path.string()};

    const AsterKV::Core::Status recoverStatus = walStorage.recover();

    const AsterKV::Core::Status firstStatus =
        walStorage.set("username", "alex");

    const AsterKV::Core::Status secondStatus =
        walStorage.set("role", "admin");

    const AsterKV::Core::Status thirdStatus =
        walStorage.remove("username");

    const auto records = AsterKV::Wal::readRecordsFromFile(path.string());

    removeIfExists(path);

    return recoverStatus.isOk() &&
           firstStatus.isOk() &&
           secondStatus.isOk() &&
           thirdStatus.isOk() &&
           records.isOk() &&
           records.value().size() == 3 &&
           records.value()[0].sequenceNumber == 1 &&
           records.value()[0].type == AsterKV::Wal::WalRecordType::Set &&
           records.value()[0].key == "username" &&
           records.value()[0].value == "alex" &&
           records.value()[1].sequenceNumber == 2 &&
           records.value()[1].type == AsterKV::Wal::WalRecordType::Set &&
           records.value()[1].key == "role" &&
           records.value()[1].value == "admin" &&
           records.value()[2].sequenceNumber == 3 &&
           records.value()[2].type == AsterKV::Wal::WalRecordType::Del &&
           records.value()[2].key == "username";
}

} // namespace

int main() {
    if (!testSetWritesWalAndUpdatesStorage()) {
        return 1;
    }

    if (!testDelWritesWalAndUpdatesStorage()) {
        return 1;
    }

    if (!testRecoverRestoresStorageState()) {
        return 1;
    }

    if (!testRejectsEmptyWalPathOnSet()) {
        return 1;
    }

    if (!testWalBackedStorageAppendsMultipleMutations()) {
        return 1;
    }

    return 0;
}
