#include <asterkv/wal/wal_file_reader.h>

#include <fstream>

#include "asterkv/wal/wal_record_codec.h"

namespace AsterKV::Wal {
    namespace {
        [[nodiscard]] Core::Status makeLineParseError(std::size_t lineNumber, const Core::Status &status) {
            std::string message = "failed to parse WAL record at line ";
            message.append(std::to_string(lineNumber));
            message.append(": ");
            message.append(status.message());

            return Core::Status::invalidArgument(std::move(message));
        }
    }

    WalFileReader::WalFileReader(std::string path) : path_(std::move(path)) {
    }

    const std::string & WalFileReader::path() const noexcept {
        return path_;
    }

    Core::Result<std::vector<WalRecord>> WalFileReader::readAll() const {
        return readRecordsFromFile(path_);
    }

    Core::Result<std::vector<WalRecord>> readRecordsFromFile(std::string_view path) {
        if (path.empty()) {
            return Core::Result<std::vector<WalRecord>>::failure(
                Core::Status::invalidArgument("WAL file path must not be empty")
            );
        }

        const std::string pathString {path};

        std::ifstream input {pathString, std::ios::binary};
        if (!input.is_open()) {
            return Core::Result<std::vector<WalRecord>>::failure(
                Core::Status::unavailable("failed to open WAL file for read")
            );
        }

        std::vector<WalRecord> records;
        std::string line;
        std::size_t lineNumber = 0;

        while (std::getline(input, line)) {
            ++lineNumber;

            if (line.empty()) {
                return Core::Result<std::vector<WalRecord>>::failure(
                    Core::Status::invalidArgument("WAL file contains empty record line")
                );
            }

            Core::Result<WalRecord> record = deserialize(line);
            if (record.isError()) {
                return Core::Result<std::vector<WalRecord>>::failure(
                    makeLineParseError(lineNumber, record.status())
                );
            }

            records.push_back(record.value());
        }

        if (input.bad()) {
            return Core::Result<std::vector<WalRecord>>::failure(
                Core::Status::invalidArgument("failed to read WAL file")
            );
        }

        return Core::Result<std::vector<WalRecord>>::success(std::move(records));
    }
}
