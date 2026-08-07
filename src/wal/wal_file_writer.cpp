#include <asterkv/wal/wal_file_writer.h>
#include <asterkv/wal/wal_record_codec.h>

#include <cerrno>
#include <cstddef>
#include <fcntl.h>
#include <string>
#include <string_view>
#include <sys/stat.h>
#include <unistd.h>
#include <utility>
#include <cstring>

namespace AsterKV::Wal {
    namespace {
        constexpr int invalidFileDescriptor = -1;

        [[nodiscard]] bool shouldRetryAfterInterrupt() noexcept {
            return errno == EINTR;
        }

        [[nodiscard]] Core::Status makeErrnoStatus(std::string_view prefix) {
            std::string message {prefix};
            message.append(": ");
            message.append(std::strerror(errno));

            return Core::Status::unavailable(std::move(message));
        }

        [[nodiscard]] Core::Status writeAll(int fileDescriptor, std::string_view data) {
            std::string_view remaining = data;

            while (!remaining.empty()) {
                const ssize_t written = ::write(fileDescriptor, remaining.data(), remaining.size());
                if (written < 0) {
                    if (shouldRetryAfterInterrupt()) {
                        continue;
                    }

                    return makeErrnoStatus("failed to write WAL record");
                }

                if (written == 0) {
                    return makeErrnoStatus("failed to make WAL write progress");
                }

                remaining.remove_prefix(static_cast<std::size_t>(written));
            }

            return Core::Status::ok();
        }

        [[nodiscard]] Core::Status closeFileDescriptor(int fileDescriptor) {
            while (::close(fileDescriptor) != 0) {
                if (shouldRetryAfterInterrupt()) {
                    continue;
                }

                return makeErrnoStatus("failed to close WAL file");
            }

            return Core::Status::ok();
        }
    }

    std::string_view walSyncPolicyToString(WalSyncPolicy policy) noexcept {
        switch (policy) {
            case WalSyncPolicy::None:
                return "none";

            case WalSyncPolicy::FsyncOnFlush:
                return "fsync_on_flush";

            case WalSyncPolicy::FsyncEveryWrite:
                return "fsync_every_write";
        }

        return "unknown";
    }

    Core::Result<WalSyncPolicy> walSyncPolicyFromString(std::string_view policy) {
        if (policy == "none") {
            return Core::Result<WalSyncPolicy>::success(WalSyncPolicy::None);
        }

        if (policy == "fsync_on_flush") {
            return Core::Result<WalSyncPolicy>::success(WalSyncPolicy::FsyncOnFlush);
        }

        if (policy == "fsync_every_write") {
            return Core::Result<WalSyncPolicy>::success(WalSyncPolicy::FsyncEveryWrite);
        }

        return Core::Result<WalSyncPolicy>::failure(Core::Status::invalidArgument("unknown WAL sync policy"));
    }

    Core::Status WalFileWriter::syncFile() {
        if (!isOpen()) {
            return Core::Status::unavailable("WAL file writer is not open");
        }

        while (::fsync(fileDescriptor_) != 0) {
            if (shouldRetryAfterInterrupt()) {
                continue;
            }

            return makeErrnoStatus("failed to fsync WAL file");
        }

        return Core::Status::ok();
    }

    WalFileWriter::WalFileWriter(std::string path, WalFileWriterOptions options) :
        filePath_(std::move(path)),
        options_(options),
        fileDescriptor_(invalidFileDescriptor),
        isClosed_(false)
    {
    }

    WalFileWriter::~WalFileWriter() {
        if (fileDescriptor_ != invalidFileDescriptor) {
            if (options_.syncPolicy != WalSyncPolicy::None) {
                static_cast<void>(::fsync(fileDescriptor_));
            }

            static_cast<void>(::close(fileDescriptor_));

            fileDescriptor_ = invalidFileDescriptor;
        }
    }

    bool WalFileWriter::isOpen() const noexcept {
        return fileDescriptor_ != invalidFileDescriptor;
    }

    Core::Status WalFileWriter::open() {
        if (filePath_.empty()) {
            return Core::Status::invalidArgument("WAL file path must not be empty");
        }

        if (isClosed_) {
            return Core::Status::unavailable("WAL file writer is closed");
        }

        if (isOpen()) {
            return Core::Status::ok();
        }

        fileDescriptor_ = ::open(
            filePath_.c_str(),
            O_WRONLY | O_CREAT | O_APPEND | O_CLOEXEC,
            S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
            );

        if (fileDescriptor_ == invalidFileDescriptor) {
            return makeErrnoStatus("failed to open WAL file for append");
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

        const Core::Status writeStatus = writeAll(fileDescriptor_, serializedRecord.value());
        if (!writeStatus.isOk()) {
            return writeStatus;
        }

        if (options_.syncPolicy == WalSyncPolicy::FsyncEveryWrite) {
            return syncFile();
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

        if (options_.syncPolicy == WalSyncPolicy::None) {
            return Core::Status::ok();
        }

        return syncFile();
    }

    Core::Status WalFileWriter::close() {
        if (isClosed_) {
            return Core::Status::ok();
        }

        if (isOpen()) {
            const Core::Status flushStatus = flush();
            if (!flushStatus.isOk()) {
                return flushStatus;
            }

            const Core::Status closeStatus = closeFileDescriptor(fileDescriptor_);
            if (!closeStatus.isOk()) {
                return closeStatus;
            }

            fileDescriptor_ = invalidFileDescriptor;
        }

        isClosed_ = true;

        return Core::Status::ok();
    }

    const std::string &WalFileWriter::path() const noexcept {
        return filePath_;
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
