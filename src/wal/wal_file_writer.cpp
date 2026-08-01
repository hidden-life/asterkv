#include <asterkv/wal/wal_file_writer.h>
#include <asterkv/wal/wal_record_codec.h>

#include <fstream>

namespace AsterKV::Wal {
    WalFileWriter::WalFileWriter(std::string path, WalFileWriterOptions options) : path_(std::move(path)), options_(options) {
    }

    const std::string &WalFileWriter::path() const noexcept {
        return path_;
    }

    const WalFileWriterOptions &WalFileWriter::options() const noexcept {
        return options_;
    }

    Core::Status WalFileWriter::appendRecord(const WalRecord &record) const {
        return appendWalRecordToFile(path_, record, options_);
    }

    Core::Status appendWalRecordToFile(std::string_view path, const WalRecord &record, WalFileWriterOptions options) {
        if(path.empty()) {
            return Core::Status::invalidArgument("WAL file path must not be empty");
        }

        const Core::Result<std::string> serialized = serialize(record);
        if (serialized.isError()) {
            return serialized.status();
        }

        const std::string filePathString {path};

        std::ofstream out {
            filePathString,
            std::ios::binary | std::ios::app
        };

        if (!out.is_open()) {
            return Core::Status::unavailable("failed to open WAL file for append");
        }

        out << serialized.value();
        if (!out.good()) {
            return Core::Status::unavailable("failed to write WAL record");
        }

        if (options.flushAfterWrite) {
            out.flush();
            if (!out.good()) {
                return Core::Status::unavailable("failed to flush WAL file");
            }
        }

        return Core::Status::ok();
    }
}
