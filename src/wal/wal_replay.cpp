#include <asterkv/wal/wal_replay.h>
#include <asterkv/wal/wal_file_reader.h>

namespace AsterKV::Wal {
    Core::Status replayRecord(const WalRecord &record, WalReplayTarget &target) {
        const Core::Status validationStatus = validateWalRecord(record);
        if (!validationStatus.isOk()) {
            return validationStatus;
        }

        switch (record.type) {
            case WalRecordType::Set:
                return target.set(record.key, record.value);

            case WalRecordType::Del:
                return target.del(record.key);
        }

        return Core::Status::invalidArgument("unknown WAL record type");
    }

    Core::Status replayRecords(const std::vector<WalRecord> &records, WalReplayTarget &target) {
        for (const WalRecord &record : records) {
            const Core::Status status = replayRecord(record, target);
            if (!status.isOk()) {
                return status;
            }
        }

        return Core::Status::ok();
    }

    Core::Status replayFile(std::string_view path, WalReplayTarget &target) {
        Core::Result<std::vector<WalRecord>> records = readRecordsFromFile(path);
        if (records.isError()) {
            return records.status();
        }

        return replayRecords(records.value(), target);
    }
}
