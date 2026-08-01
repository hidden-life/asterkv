#include <asterkv/wal/wal_record.h>

#include <utility>

namespace AsterKV::Wal {
    std::string_view walRecordTypeToString(WalRecordType type) noexcept {
        switch (type) {
            case WalRecordType::Set:
                return "set";
            case WalRecordType::Del:
                return "del";
        }

        return "unknown";
    }

    Core::Status validateWalRecord(const WalRecord &record) {
        if (record.sequenceNumber == 0) {
            return Core::Status::invalidArgument("WAL record sequence number must be greater than zero");
        }

        if (record.key.empty()) {
            return Core::Status::invalidArgument("WAL record key must not be empty");
        }

        switch (record.type) {
            case WalRecordType::Set:
                if (record.value.empty()) {
                    return Core::Status::invalidArgument("WAL set record value must not be empty");
                }

                return Core::Status::ok();

            case WalRecordType::Del:
                if (!record.value.empty()) {
                    return Core::Status::invalidArgument("WAL del record must be empty");
                }

                return Core::Status::ok();
        }

        return Core::Status::invalidArgument("unknown WAL record type");
    }

    WalRecord makeSetRecord(WalSequenceNumber number, std::string key, std::string value) {
        return WalRecord {
            .sequenceNumber = number,
            .type = WalRecordType::Set,
            .key = std::move(key),
            .value = std::move(value)
        };
    }

    WalRecord makeDelRecord(WalSequenceNumber number, std::string key) {
        return WalRecord {
            .sequenceNumber = number,
            .type = WalRecordType::Del,
            .key = std::move(key),
            .value = {},
        };
    }
}
