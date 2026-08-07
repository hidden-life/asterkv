#include <asterkv/wal/wal_file_writer.h>

#include <asterkv/wal/wal_record.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>

namespace {

    [[nodiscard]] std::filesystem::path makeTempWalPath(std::string_view suffix) {
        const auto now = std::chrono::steady_clock::now()
                             .time_since_epoch()
                             .count();

        std::string fileName = "asterkv_wal_writer_test_";
        fileName.append(std::to_string(now));
        fileName.push_back('_');
        fileName.append(suffix);
        fileName.append(".wal");

        return std::filesystem::temp_directory_path() / fileName;
    }

    [[nodiscard]] std::string readFile(const std::filesystem::path& path) {
        std::ifstream input{path, std::ios::binary};

        return std::string{
            std::istreambuf_iterator<char>{input},
            std::istreambuf_iterator<char>{},
        };
    }

    void removeIfExists(const std::filesystem::path& path) {
        std::error_code error;
        std::filesystem::remove(path, error);
    }

    [[nodiscard]] bool testAppendsSingleSetRecord() {
        const std::filesystem::path path = makeTempWalPath("single_set");
        removeIfExists(path);

        const AsterKV::Wal::WalRecord record =
            AsterKV::Wal::makeSetRecord(1, "username", "jackson");

        AsterKV::Wal::WalFileWriter writer{path.string()};

        const AsterKV::Core::Status status = writer.appendRecord(record);

        const std::string content = readFile(path);
        removeIfExists(path);

        return status.isOk() &&
               content ==
                   "AKVWAL1 1 set 757365726e616d65 6a61636b736f6e\n";
    }

    [[nodiscard]] bool testAppendsMultipleRecordsInOrder() {
        const std::filesystem::path path = makeTempWalPath("multiple");
        removeIfExists(path);

        AsterKV::Wal::WalFileWriter writer{path.string()};

        const AsterKV::Core::Status firstStatus =
            writer.appendRecord(
                AsterKV::Wal::makeSetRecord(1, "username", "jackson"));

        const AsterKV::Core::Status secondStatus =
            writer.appendRecord(
                AsterKV::Wal::makeDelRecord(2, "username"));

        const std::string content = readFile(path);
        removeIfExists(path);

        return firstStatus.isOk() &&
               secondStatus.isOk() &&
               content ==
                   "AKVWAL1 1 set 757365726e616d65 6a61636b736f6e\n"
                   "AKVWAL1 2 del 757365726e616d65 -\n";
    }

    [[nodiscard]] bool testAppendHelperWritesRecord() {
        const std::filesystem::path path = makeTempWalPath("helper");
        removeIfExists(path);

        const AsterKV::Core::Status status =
            AsterKV::Wal::appendWalRecordToFile(
                path.string(),
                AsterKV::Wal::makeSetRecord(1, "key", "value"));

        const std::string content = readFile(path);
        removeIfExists(path);

        return status.isOk() &&
               content == "AKVWAL1 1 set 6b6579 76616c7565\n";
    }

    [[nodiscard]] bool testCanUseNoSyncPolicy() {
        const std::filesystem::path path = makeTempWalPath("no_sync");
        removeIfExists(path);

        const AsterKV::Wal::WalFileWriterOptions options{
            .syncPolicy = AsterKV::Wal::WalSyncPolicy::None,
        };

        AsterKV::Wal::WalFileWriter writer{path.string(), options};

        const AsterKV::Core::Status status =
            writer.appendRecord(
                AsterKV::Wal::makeSetRecord(1, "key", "value"));

        const AsterKV::Core::Status closeStatus = writer.close();

        const std::string content = readFile(path);
        removeIfExists(path);

        return status.isOk() &&
               closeStatus.isOk() &&
               writer.options().syncPolicy == AsterKV::Wal::WalSyncPolicy::None &&
               content == "AKVWAL1 1 set 6b6579 76616c7565\n";
    }

    [[nodiscard]] bool testRejectsEmptyPath() {
        const AsterKV::Core::Status status =
            AsterKV::Wal::appendWalRecordToFile(
                "",
                AsterKV::Wal::makeSetRecord(1, "key", "value"));

        return !status.isOk();
    }

    [[nodiscard]] bool testRejectsInvalidRecordBeforeCreatingFile() {
        const std::filesystem::path path = makeTempWalPath("invalid_record");
        removeIfExists(path);

        const AsterKV::Core::Status status =
            AsterKV::Wal::appendWalRecordToFile(
                path.string(),
                AsterKV::Wal::makeSetRecord(0, "key", "value"));

        const bool exists = std::filesystem::exists(path);
        removeIfExists(path);

        return !status.isOk() && !exists;
    }

    [[nodiscard]] bool testRejectsPathInsideMissingDirectory() {
        const std::filesystem::path path =
            std::filesystem::temp_directory_path() /
            "asterkv_missing_wal_writer_directory" /
            "wal.log";

        const AsterKV::Core::Status status =
            AsterKV::Wal::appendWalRecordToFile(
                path.string(),
                AsterKV::Wal::makeSetRecord(1, "key", "value"));

        return !status.isOk();
    }

    [[nodiscard]] bool testExposesFilePath() {
        const AsterKV::Wal::WalFileWriter writer{"test.wal"};

        return writer.path() == "test.wal";
    }

        [[nodiscard]] bool testWriterKeepsAppendHandleOpen() {
        const std::filesystem::path path = makeTempWalPath("open_handle");
        removeIfExists(path);

        AsterKV::Wal::WalFileWriter writer{path.string()};

        const AsterKV::Core::Status openStatus = writer.open();

        const bool openAfterOpen = writer.isOpen();

        const AsterKV::Core::Status appendStatus =
            writer.appendRecord(
                AsterKV::Wal::makeSetRecord(1, "username", "alex"));

        const bool openAfterAppend = writer.isOpen();

        const AsterKV::Core::Status closeStatus = writer.close();

        const bool openAfterClose = writer.isOpen();

        const std::string content = readFile(path);
        removeIfExists(path);

        return openStatus.isOk() &&
               appendStatus.isOk() &&
               closeStatus.isOk() &&
               openAfterOpen &&
               openAfterAppend &&
               !openAfterClose &&
               content == "AKVWAL1 1 set 757365726e616d65 616c6578\n";
    }

    [[nodiscard]] bool testWriterAppendsMultipleRecordsThroughOneHandle() {
        const std::filesystem::path path = makeTempWalPath("multi_handle");
        removeIfExists(path);

        AsterKV::Wal::WalFileWriter writer{path.string()};

        const AsterKV::Core::Status firstStatus =
            writer.appendRecord(
                AsterKV::Wal::makeSetRecord(1, "username", "alex"));

        const AsterKV::Core::Status secondStatus =
            writer.appendRecord(
                AsterKV::Wal::makeSetRecord(2, "role", "admin"));

        const AsterKV::Core::Status thirdStatus =
            writer.appendRecord(
                AsterKV::Wal::makeDelRecord(3, "username"));

        const AsterKV::Core::Status closeStatus = writer.close();

        const std::string content = readFile(path);
        removeIfExists(path);

        return firstStatus.isOk() &&
               secondStatus.isOk() &&
               thirdStatus.isOk() &&
               closeStatus.isOk() &&
               content ==
                   "AKVWAL1 1 set 757365726e616d65 616c6578\n"
                   "AKVWAL1 2 set 726f6c65 61646d696e\n"
                   "AKVWAL1 3 del 757365726e616d65 -\n";
    }

    [[nodiscard]] bool testWriterRejectsAppendAfterClose() {
        const std::filesystem::path path = makeTempWalPath("append_after_close");
        removeIfExists(path);

        AsterKV::Wal::WalFileWriter writer{path.string()};

        const AsterKV::Core::Status closeStatus = writer.close();

        const AsterKV::Core::Status appendStatus =
            writer.appendRecord(
                AsterKV::Wal::makeSetRecord(1, "username", "alex"));

        removeIfExists(path);

        return closeStatus.isOk() && !appendStatus.isOk();
    }

    [[nodiscard]] bool testWriterRejectsFlushAfterClose() {
        const std::filesystem::path path = makeTempWalPath("flush_after_close");
        removeIfExists(path);

        AsterKV::Wal::WalFileWriter writer{path.string()};

        const AsterKV::Core::Status closeStatus = writer.close();
        const AsterKV::Core::Status flushStatus = writer.flush();

        removeIfExists(path);

        return closeStatus.isOk() && !flushStatus.isOk();
    }

    [[nodiscard]] bool testDefaultSyncPolicyIsFsyncEveryWrite() {
    const AsterKV::Wal::WalFileWriterOptions options{};

    return options.syncPolicy ==
           AsterKV::Wal::WalSyncPolicy::FsyncEveryWrite;
}

[[nodiscard]] bool testSyncPolicyToString() {
    return AsterKV::Wal::walSyncPolicyToString(
               AsterKV::Wal::WalSyncPolicy::None) == "none" &&
           AsterKV::Wal::walSyncPolicyToString(
               AsterKV::Wal::WalSyncPolicy::FsyncOnFlush) ==
               "fsync_on_flush" &&
           AsterKV::Wal::walSyncPolicyToString(
               AsterKV::Wal::WalSyncPolicy::FsyncEveryWrite) ==
               "fsync_every_write";
}

[[nodiscard]] bool testParsesSyncPolicy() {
    const auto none = AsterKV::Wal::walSyncPolicyFromString("none");
    const auto onFlush =
        AsterKV::Wal::walSyncPolicyFromString("fsync_on_flush");
    const auto everyWrite =
        AsterKV::Wal::walSyncPolicyFromString("fsync_every_write");

    return none.isOk() &&
           none.value() == AsterKV::Wal::WalSyncPolicy::None &&
           onFlush.isOk() &&
           onFlush.value() == AsterKV::Wal::WalSyncPolicy::FsyncOnFlush &&
           everyWrite.isOk() &&
           everyWrite.value() ==
               AsterKV::Wal::WalSyncPolicy::FsyncEveryWrite;
}

[[nodiscard]] bool testRejectsUnknownSyncPolicy() {
    return AsterKV::Wal::walSyncPolicyFromString("always").isError();
}

[[nodiscard]] bool testFsyncOnFlushPolicyWritesRecord() {
    const std::filesystem::path path = makeTempWalPath("fsync_on_flush");
    removeIfExists(path);

    const AsterKV::Wal::WalFileWriterOptions options{
        .syncPolicy = AsterKV::Wal::WalSyncPolicy::FsyncOnFlush,
    };

    AsterKV::Wal::WalFileWriter writer{path.string(), options};

    const AsterKV::Core::Status appendStatus =
        writer.appendRecord(
            AsterKV::Wal::makeSetRecord(1, "username", "alex"));

    const AsterKV::Core::Status flushStatus = writer.flush();
    const AsterKV::Core::Status closeStatus = writer.close();

    const std::string content = readFile(path);
    removeIfExists(path);

    return appendStatus.isOk() &&
           flushStatus.isOk() &&
           closeStatus.isOk() &&
           content == "AKVWAL1 1 set 757365726e616d65 616c6578\n";
}

[[nodiscard]] bool testFsyncEveryWritePolicyWritesRecord() {
    const std::filesystem::path path = makeTempWalPath("fsync_every_write");
    removeIfExists(path);

    const AsterKV::Wal::WalFileWriterOptions options{
        .syncPolicy = AsterKV::Wal::WalSyncPolicy::FsyncEveryWrite,
    };

    AsterKV::Wal::WalFileWriter writer{path.string(), options};

    const AsterKV::Core::Status appendStatus =
        writer.appendRecord(
            AsterKV::Wal::makeSetRecord(1, "username", "alex"));

    const AsterKV::Core::Status closeStatus = writer.close();

    const std::string content = readFile(path);
    removeIfExists(path);

    return appendStatus.isOk() &&
           closeStatus.isOk() &&
           content == "AKVWAL1 1 set 757365726e616d65 616c6578\n";
}

} // namespace

int main() {
    if (!testAppendsSingleSetRecord()) {
        return 1;
    }

    if (!testAppendsMultipleRecordsInOrder()) {
        return 1;
    }

    if (!testAppendHelperWritesRecord()) {
        return 1;
    }

    if (!testCanUseNoSyncPolicy()) {
        return 1;
    }

    if (!testRejectsEmptyPath()) {
        return 1;
    }

    if (!testRejectsInvalidRecordBeforeCreatingFile()) {
        return 1;
    }

    if (!testRejectsPathInsideMissingDirectory()) {
        return 1;
    }

    if (!testExposesFilePath()) {
        return 1;
    }

    if (!testWriterKeepsAppendHandleOpen()) {
        return 1;
    }

    if (!testWriterAppendsMultipleRecordsThroughOneHandle()) {
        return 1;
    }

    if (!testWriterRejectsAppendAfterClose()) {
        return 1;
    }

    if (!testWriterRejectsFlushAfterClose()) {
        return 1;
    }

    if (!testDefaultSyncPolicyIsFsyncEveryWrite()) {
        return 1;
    }

    if (!testSyncPolicyToString()) {
        return 1;
    }

    if (!testParsesSyncPolicy()) {
        return 1;
    }

    if (!testRejectsUnknownSyncPolicy()) {
        return 1;
    }

    if (!testFsyncOnFlushPolicyWritesRecord()) {
        return 1;
    }

    if (!testFsyncEveryWritePolicyWritesRecord()) {
        return 1;
    }

    return 0;
}
