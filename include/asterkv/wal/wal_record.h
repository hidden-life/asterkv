#ifndef ASTERKV_WAL_WAL_RECORD_H
#define ASTERKV_WAL_WAL_RECORD_H

#include <cstdint>
#include <string>

#include <asterkv/core/status.h>

namespace AsterKV::Wal {
    using WalSequenceNumber = std::uint64_t;

    enum class WalRecordType {
        Set,
        Del,
    };

    struct WalRecord final {
        WalSequenceNumber sequenceNumber;
        WalRecordType type;
        std::string key;
        std::string value;
    };

    [[nodiscard]] std::string_view walRecordTypeToString(WalRecordType type) noexcept;

    [[nodiscard]] Core::Status validateWalRecord(const WalRecord &record);

    [[nodiscard]] WalRecord makeSetRecord(WalSequenceNumber number, std::string key, std::string value);
    [[nodiscard]] WalRecord makeDelRecord(WalSequenceNumber number, std::string key);
}

#endif //ASTERKV_WAL_WAL_RECORD_H
