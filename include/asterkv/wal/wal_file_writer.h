#ifndef ASTERKV_WAL_WAL_FILE_WRITER_H
#define ASTERKV_WAL_WAL_FILE_WRITER_H

#include <string>
#include <string_view>
#include <fstream>

#include <asterkv/core/result.h>
#include <asterkv/wal/wal_record.h>

namespace AsterKV::Wal {
    struct WalFileWriterOptions final {
        bool flushAfterWrite = true;
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
        std::string path_;
        WalFileWriterOptions options_;
        std::ofstream output_;
        bool isClosed_;
    };

    [[nodiscard]] Core::Status appendWalRecordToFile(
        std::string_view path,
        const WalRecord &record,
        WalFileWriterOptions options = {}
        );
}

#endif //ASTERKV_WAL_WAL_FILE_WRITER_H
