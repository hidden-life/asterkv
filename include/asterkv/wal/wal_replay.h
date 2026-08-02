#ifndef ASTERKV_WAL_WAL_REPLAY_H
#define ASTERKV_WAL_WAL_REPLAY_H

#include <asterkv/core/status.h>
#include <asterkv/wal/wal_record.h>

#include <vector>

namespace AsterKV::Wal {
    class WalReplayTarget {
    public:
        virtual ~WalReplayTarget() = default;

        [[nodiscard]] virtual Core::Status set(std::string_view key, std::string_view value) = 0;
        [[nodiscard]] virtual Core::Status del(std::string_view value) = 0;
    };

    [[nodiscard]] Core::Status replayRecord(const WalRecord &record, WalReplayTarget &target);
    [[nodiscard]] Core::Status replayRecords(const std::vector<WalRecord> &records, WalReplayTarget &target);
    [[nodiscard]] Core::Status replayFile(std::string_view path, WalReplayTarget &target);
}

#endif //ASTERKV_WAL_WAL_REPLAY_H
