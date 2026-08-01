#ifndef ASTERKV_WAL_WAL_FILE_READER_H
#define ASTERKV_WAL_WAL_FILE_READER_H

#include <string>
#include <string_view>
#include <vector>

#include <asterkv/core/result.h>
#include <asterkv/wal/wal_record.h>

namespace AsterKV::Wal {
    class WalFileReader final {
    public:
        explicit WalFileReader(std::string path);

        [[nodiscard]] const std::string &path() const noexcept;
        [[nodiscard]] Core::Result<std::vector<WalRecord>> readAll() const;

    private:
        std::string path_;
    };

    [[nodiscard]] Core::Result<std::vector<WalRecord>> readRecordsFromFile(std::string_view path);
}

#endif //ASTERKV_WAL_WAL_FILE_READER_H
