#ifndef ASTERKV_WAL_WAL_FILE_WRITER_H
#define ASTERKV_WAL_WAL_FILE_WRITER_H

#include <string>
#include <string_view>
#include <fstream>

#include <asterkv/core/result.h>
#include <asterkv/core/status.h>
#include <asterkv/wal/wal_record.h>

namespace AsterKV::Wal {
    enum class WalSyncPolicy {
        None,
        FsyncOnFlush,
        FsyncEveryWrite,
    };

    [[nodiscard]] std::string_view walSyncPolicyToString(WalSyncPolicy policy) noexcept;

    [[nodiscard]] Core::Result<WalSyncPolicy> walSyncPolicyFromString(std::string_view policy);

    struct WalFileWriterOptions final {
        WalSyncPolicy syncPolicy = WalSyncPolicy::FsyncEveryWrite;
    };

    class WalFileWriter final {
    public:
        explicit WalFileWriter(std::string path, WalFileWriterOptions options = {});

        WalFileWriter(const WalFileWriter&) = delete;
        WalFileWriter& operator=(const WalFileWriter&) = delete;

        WalFileWriter(WalFileWriter&&) = delete;
        WalFileWriter& operator=(WalFileWriter&&) = delete;

        ~WalFileWriter();

        [[nodiscard]] const std::string &path() const noexcept;
        [[nodiscard]] const WalFileWriterOptions &options() const noexcept;

        [[nodiscard]] bool isOpen() const noexcept;

        [[nodiscard]] Core::Status open();
        [[nodiscard]] Core::Status appendRecord(const WalRecord &record);
        [[nodiscard]] Core::Status flush();
        [[nodiscard]] Core::Status close();

    private:
        [[nodiscard]] Core::Status syncFile();

        std::string filePath_;
        WalFileWriterOptions options_;
        int fileDescriptor_;
        bool isClosed_;
    };

    [[nodiscard]] Core::Status appendWalRecordToFile(
        std::string_view path,
        const WalRecord &record,
        WalFileWriterOptions options = {}
        );
}

#endif //ASTERKV_WAL_WAL_FILE_WRITER_H
