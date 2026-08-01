#ifndef ASTERKV_WAL_WAL_FILE_WRITER_H
#define ASTERKV_WAL_WAL_FILE_WRITER_H

#include <string>
#include <string_view>

#include <asterkv/core/result.h>
#include <asterkv/wal/wal_record.h>

namespace AsterKV::Wal {
    struct WalFileWriterOptions final {
        bool flushAfterWrite = true;
    };

    class WalFileWriter final {
    public:
        explicit WalFileWriter(std::string path, WalFileWriterOptions options = {});

        [[nodiscard]] const std::string &path() const noexcept;
        [[nodiscard]] const WalFileWriterOptions &options() const noexcept;

        [[nodiscard]] Core::Status appendRecord(const WalRecord &record) const;

    private:
        std::string path_;
        WalFileWriterOptions options_;
    };

    [[nodiscard]] Core::Status appendWalRecordToFile(std::string_view path, const WalRecord &record, WalFileWriterOptions options = {});
}

#endif //ASTERKV_WAL_WAL_FILE_WRITER_H
