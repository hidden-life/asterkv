#include <asterkv/wal/wal_replay.h>
#include <asterkv/wal/wal_file_writer.h>

#include <chrono>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace {

class FakeReplayTarget final : public AsterKV::Wal::WalReplayTarget {
public:
    [[nodiscard]] AsterKV::Core::Status set(
        std::string_view key,
        std::string_view value) override {
        events.push_back("set:" + std::string{key} + "=" + std::string{value});
        return AsterKV::Core::Status::ok();
    }

    [[nodiscard]] AsterKV::Core::Status del(
        std::string_view key) override {
        events.push_back("del:" + std::string{key});
        return AsterKV::Core::Status::ok();
    }

    std::vector<std::string> events;
};

[[nodiscard]] std::filesystem::path makeTempWalPath(std::string_view suffix) {
    const auto now = std::chrono::steady_clock::now()
                         .time_since_epoch()
                         .count();

    std::string fileName = "asterkv_wal_replay_test_";
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

[[nodiscard]] bool testReplaysSetRecord() {
    FakeReplayTarget target;

    const AsterKV::Core::Status status =
        AsterKV::Wal::replayRecord(
            AsterKV::Wal::makeSetRecord(1, "username", "alex"),
            target);

    return status.isOk() &&
           target.events.size() == 1 &&
           target.events[0] == "set:username=alex";
}

[[nodiscard]] bool testReplaysDelRecord() {
    FakeReplayTarget target;

    const AsterKV::Core::Status status =
        AsterKV::Wal::replayRecord(
            AsterKV::Wal::makeDelRecord(2, "username"),
            target);

    return status.isOk() &&
           target.events.size() == 1 &&
           target.events[0] == "del:username";
}

[[nodiscard]] bool testReplaysRecordsInOrder() {
    FakeReplayTarget target;

    const std::vector<AsterKV::Wal::WalRecord> records{
        AsterKV::Wal::makeSetRecord(1, "username", "alex"),
        AsterKV::Wal::makeDelRecord(2, "username"),
        AsterKV::Wal::makeSetRecord(3, "username", "admin"),
    };

    const AsterKV::Core::Status status =
        AsterKV::Wal::replayRecords(records, target);

    return status.isOk() &&
           target.events.size() == 3 &&
           target.events[0] == "set:username=alex" &&
           target.events[1] == "del:username" &&
           target.events[2] == "set:username=admin";
}

[[nodiscard]] bool testRejectsInvalidRecord() {
    FakeReplayTarget target;

    const AsterKV::Wal::WalRecord invalidRecord =
        AsterKV::Wal::makeSetRecord(0, "username", "alex");

    const AsterKV::Core::Status status =
        AsterKV::Wal::replayRecord(invalidRecord, target);

    return !status.isOk() && target.events.empty();
}

[[nodiscard]] bool testReplaysWalFile() {
    const std::filesystem::path path = makeTempWalPath("file");
    removeIfExists(path);

    const AsterKV::Core::Status firstStatus =
        AsterKV::Wal::appendWalRecordToFile(
            path.string(),
            AsterKV::Wal::makeSetRecord(1, "username", "alex"));

    const AsterKV::Core::Status secondStatus =
        AsterKV::Wal::appendWalRecordToFile(
            path.string(),
            AsterKV::Wal::makeDelRecord(2, "username"));

    FakeReplayTarget target;

    const AsterKV::Core::Status replayStatus =
        AsterKV::Wal::replayFile(path.string(), target);

    removeIfExists(path);

    return firstStatus.isOk() &&
           secondStatus.isOk() &&
           replayStatus.isOk() &&
           target.events.size() == 2 &&
           target.events[0] == "set:username=alex" &&
           target.events[1] == "del:username";
}

} // namespace

int main() {
    if (!testReplaysSetRecord()) {
        return 1;
    }

    if (!testReplaysDelRecord()) {
        return 1;
    }

    if (!testReplaysRecordsInOrder()) {
        return 1;
    }

    if (!testRejectsInvalidRecord()) {
        return 1;
    }

    if (!testReplaysWalFile()) {
        return 1;
    }

    return 0;
}
