#include <asterkv/wal/wal_file_writer.h>
#include <asterkv/wal/wal_record_codec.h>

#include <fstream>

namespace AsterKV::Wal {
    WalFileWriter::WalFileWriter(std::string path, WalFileWriterOptions options) :
        path_(std::move(path)),
        options_(options),
        output_(),
        isClosed_(false)
    {
    }

    WalFileWriter::~WalFileWriter() {
        if (output_.is_open()) {
            output_.flush();
            output_.close();
        }
    }

    bool WalFileWriter::isOpen() const noexcept {
        return output_.is_open();
    }

    Core::Status WalFileWriter::open() {
        if (path_.empty()) {
            return Core::Status::invalidArgument("WAL file path must not be empty");
        }

        if (isClosed_) {
            return Core::Status::unavailable("WAL file writer is closed");
        }

        if (output_.is_open()) {
            return Core::Status::ok();
        }

        output_.open(path_, std::ios::binary | std::ios::app);

        if (!output_.is_open()) {
            return Core::Status::unavailable("failed to open WAL file for append");
        }

        return Core::Status::ok();
    }

    Core::Status WalFileWriter::appendRecord(const WalRecord &record) {
        if (isClosed_) {
            return Core::Status::unavailable("WAL file writer is closed");
        }

        const Core::Result<std::string> serializedRecord = serialize(record);

        if (serializedRecord.isError()) {
            return serializedRecord.status();
        }

        const Core::Status openStatus = open();

        if (!openStatus.isOk()) {
            return openStatus;
        }

        output_ << serializedRecord.value();

        if (!output_.good()) {
            return Core::Status::unavailable("failed to write WAL record");
        }

        if (options_.flushAfterWrite) {
            return flush();
        }

        return Core::Status::ok();
    }

    Core::Status WalFileWriter::flush() {
        if (isClosed_) {
            return Core::Status::unavailable("WAL file writer is closed");
        }

        const Core::Status openStatus = open();
        if (!openStatus.isOk()) {
            return openStatus;
        }

        output_.flush();
        if (!output_.good()) {
            return Core::Status::unavailable("failed to flush WAL file");
        }

        return Core::Status::ok();
    }

    Core::Status WalFileWriter::close() {
        if (isClosed_) {
            return Core::Status::ok();
        }

        if (output_.is_open()) {
            output_.flush();
            if (!output_.good()) {
                return Core::Status::unavailable("failed to flush WAL record before close");
            }

            output_.close();

            if (output_.fail()) {
                return Core::Status::unavailable("failed to close WAL file");
            }
        }

        isClosed_ = true;

        return Core::Status::ok();
    }

    const std::string &WalFileWriter::path() const noexcept {
        return path_;
    }

    const WalFileWriterOptions &WalFileWriter::options() const noexcept {
        return options_;
    }

    Core::Status appendWalRecordToFile(std::string_view path, const WalRecord &record, WalFileWriterOptions options) {
        WalFileWriter writer {std::string(path), options};
        const Core::Status appendStatus = writer.appendRecord(record);
        if (!appendStatus.isOk()) {
            return appendStatus;
        }

        return writer.close();
    }
}
