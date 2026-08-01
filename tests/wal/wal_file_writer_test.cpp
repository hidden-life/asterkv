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

    const AsterKV::Wal::WalFileWriter writer{path.string()};

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

    const AsterKV::Wal::WalFileWriter writer{path.string()};

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

[[nodiscard]] bool testCanDisableFlushAfterWrite() {
    const std::filesystem::path path = makeTempWalPath("no_flush");
    removeIfExists(path);

    const AsterKV::Wal::WalFileWriterOptions options{
        .flushAfterWrite = false,
    };

    const AsterKV::Wal::WalFileWriter writer{path.string(), options};

    const AsterKV::Core::Status status =
        writer.appendRecord(
            AsterKV::Wal::makeSetRecord(1, "key", "value"));

    const std::string content = readFile(path);
    removeIfExists(path);

    return status.isOk() &&
           writer.options().flushAfterWrite == false &&
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

    if (!testCanDisableFlushAfterWrite()) {
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

    return 0;
}
