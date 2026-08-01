#include <asterkv/wal/wal_file_reader.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>

namespace {

[[nodiscard]] std::filesystem::path makeTempWalPath(std::string_view suffix) {
    const auto now = std::chrono::steady_clock::now()
                         .time_since_epoch()
                         .count();

    std::string fileName = "asterkv_wal_reader_test_";
    fileName.append(std::to_string(now));
    fileName.push_back('_');
    fileName.append(suffix);
    fileName.append(".wal");

    return std::filesystem::temp_directory_path() / fileName;
}

void writeFile(const std::filesystem::path& path, std::string_view content) {
    std::ofstream output{path, std::ios::binary};
    output << content;
}

void removeIfExists(const std::filesystem::path& path) {
    std::error_code error;
    std::filesystem::remove(path, error);
}

[[nodiscard]] bool testReadsEmptyFile() {
    const std::filesystem::path path = makeTempWalPath("empty");
    removeIfExists(path);
    writeFile(path, "");

    const auto records = AsterKV::Wal::readRecordsFromFile(path.string());

    removeIfExists(path);

    return records.isOk() && records.value().empty();
}

[[nodiscard]] bool testReadsSingleSetRecord() {
    const std::filesystem::path path = makeTempWalPath("single_set");
    removeIfExists(path);

    writeFile(
        path,
        "AKVWAL1 1 set 757365726e616d65 6a61636b736f6e\n");

    const auto records = AsterKV::Wal::readRecordsFromFile(path.string());

    removeIfExists(path);

    return records.isOk() &&
           records.value().size() == 1 &&
           records.value()[0].sequenceNumber == 1 &&
           records.value()[0].type == AsterKV::Wal::WalRecordType::Set &&
           records.value()[0].key == "username" &&
           records.value()[0].value == "jackson";
}

[[nodiscard]] bool testReadsMultipleRecordsInOrder() {
    const std::filesystem::path path = makeTempWalPath("multiple");
    removeIfExists(path);

    writeFile(
        path,
        "AKVWAL1 1 set 757365726e616d65 6a61636b736f6e\n"
        "AKVWAL1 2 del 757365726e616d65 -\n");

    const auto records = AsterKV::Wal::readRecordsFromFile(path.string());

    removeIfExists(path);

    return records.isOk() &&
           records.value().size() == 2 &&
           records.value()[0].sequenceNumber == 1 &&
           records.value()[0].type == AsterKV::Wal::WalRecordType::Set &&
           records.value()[0].key == "username" &&
           records.value()[0].value == "jackson" &&
           records.value()[1].sequenceNumber == 2 &&
           records.value()[1].type == AsterKV::Wal::WalRecordType::Del &&
           records.value()[1].key == "username" &&
           records.value()[1].value.empty();
}

[[nodiscard]] bool testReadsCrLfRecords() {
    const std::filesystem::path path = makeTempWalPath("crlf");
    removeIfExists(path);

    writeFile(
        path,
        "AKVWAL1 1 set 6b6579 76616c7565\r\n"
        "AKVWAL1 2 del 6b6579 -\r\n");

    const auto records = AsterKV::Wal::readRecordsFromFile(path.string());

    removeIfExists(path);

    return records.isOk() &&
           records.value().size() == 2 &&
           records.value()[0].key == "key" &&
           records.value()[0].value == "value" &&
           records.value()[1].key == "key" &&
           records.value()[1].value.empty();
}

[[nodiscard]] bool testReaderClassReadsRecords() {
    const std::filesystem::path path = makeTempWalPath("reader_class");
    removeIfExists(path);

    writeFile(path, "AKVWAL1 1 set 6b6579 76616c7565\n");

    const AsterKV::Wal::WalFileReader reader{path.string()};
    const auto records = reader.readAll();

    removeIfExists(path);

    return reader.path() == path.string() &&
           records.isOk() &&
           records.value().size() == 1 &&
           records.value()[0].key == "key" &&
           records.value()[0].value == "value";
}

[[nodiscard]] bool testRejectsEmptyPath() {
    const auto records = AsterKV::Wal::readRecordsFromFile("");

    return records.isError();
}

[[nodiscard]] bool testRejectsMissingFile() {
    const std::filesystem::path path = makeTempWalPath("missing");
    removeIfExists(path);

    const auto records = AsterKV::Wal::readRecordsFromFile(path.string());

    return records.isError();
}

[[nodiscard]] bool testRejectsInvalidRecordLine() {
    const std::filesystem::path path = makeTempWalPath("invalid_record");
    removeIfExists(path);

    writeFile(path, "BADWAL1 1 set 6b6579 76616c7565\n");

    const auto records = AsterKV::Wal::readRecordsFromFile(path.string());

    removeIfExists(path);

    return records.isError();
}

[[nodiscard]] bool testRejectsEmptyRecordLine() {
    const std::filesystem::path path = makeTempWalPath("empty_line");
    removeIfExists(path);

    writeFile(
        path,
        "AKVWAL1 1 set 6b6579 76616c7565\n"
        "\n");

    const auto records = AsterKV::Wal::readRecordsFromFile(path.string());

    removeIfExists(path);

    return records.isError();
}

[[nodiscard]] bool testRejectsSecondInvalidRecordLine() {
    const std::filesystem::path path = makeTempWalPath("second_invalid");
    removeIfExists(path);

    writeFile(
        path,
        "AKVWAL1 1 set 6b6579 76616c7565\n"
        "AKVWAL1 0 set 6b6579 76616c7565\n");

    const auto records = AsterKV::Wal::readRecordsFromFile(path.string());

    removeIfExists(path);

    return records.isError();
}

} // namespace

int main() {
    if (!testReadsEmptyFile()) {
        return 1;
    }

    if (!testReadsSingleSetRecord()) {
        return 1;
    }

    if (!testReadsMultipleRecordsInOrder()) {
        return 1;
    }

    if (!testReadsCrLfRecords()) {
        return 1;
    }

    if (!testReaderClassReadsRecords()) {
        return 1;
    }

    if (!testRejectsEmptyPath()) {
        return 1;
    }

    if (!testRejectsMissingFile()) {
        return 1;
    }

    if (!testRejectsInvalidRecordLine()) {
        return 1;
    }

    if (!testRejectsEmptyRecordLine()) {
        return 1;
    }

    if (!testRejectsSecondInvalidRecordLine()) {
        return 1;
    }

    return 0;
}
