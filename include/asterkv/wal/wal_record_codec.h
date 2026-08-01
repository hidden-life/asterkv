#ifndef ASTERKV_WAL_WAL_RECORD_CODEC_H
#define ASTERKV_WAL_WAL_RECORD_CODEC_H

#include <asterkv/core/result.h>
#include <asterkv/wal/wal_record.h>

#include <string>
#include <string_view>

namespace AsterKV::Wal {
    constexpr std::string_view currentWalRecordFormatVersion = "AKVWAL1";

    [[nodiscard]] Core::Result<std::string> serialize(const WalRecord &record);
    [[nodiscard]] Core::Result<WalRecord> deserialize(std::string_view serialized);
}

#endif //ASTERKV_WAL_WAL_RECORD_CODEC_H
