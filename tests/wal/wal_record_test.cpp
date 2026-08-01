#include <asterkv/wal/wal_record.h>

namespace {

[[nodiscard]] bool testMakesSetRecord() {
    const AsterKV::Wal::WalRecord record =
        AsterKV::Wal::makeSetRecord(1, "username", "alex");

    return record.sequenceNumber == 1 &&
           record.type == AsterKV::Wal::WalRecordType::Set &&
           record.key == "username" &&
           record.value == "alex";
}

[[nodiscard]] bool testMakesDelRecord() {
    const AsterKV::Wal::WalRecord record =
        AsterKV::Wal::makeDelRecord(2, "username");

    return record.sequenceNumber == 2 &&
           record.type == AsterKV::Wal::WalRecordType::Del &&
           record.key == "username" &&
           record.value.empty();
}

[[nodiscard]] bool testRecordTypeToString() {
    return AsterKV::Wal::walRecordTypeToString(
               AsterKV::Wal::WalRecordType::Set) == "set" &&
           AsterKV::Wal::walRecordTypeToString(
               AsterKV::Wal::WalRecordType::Del) == "del";
}

[[nodiscard]] bool testValidatesSetRecord() {
    const AsterKV::Wal::WalRecord record =
        AsterKV::Wal::makeSetRecord(1, "username", "alex");

    return AsterKV::Wal::validateWalRecord(record).isOk();
}

[[nodiscard]] bool testValidatesDelRecord() {
    const AsterKV::Wal::WalRecord record =
        AsterKV::Wal::makeDelRecord(1, "username");

    return AsterKV::Wal::validateWalRecord(record).isOk();
}

[[nodiscard]] bool testRejectsZeroSequenceNumber() {
    const AsterKV::Wal::WalRecord record =
        AsterKV::Wal::makeSetRecord(0, "username", "alex");

    return !AsterKV::Wal::validateWalRecord(record).isOk();
}

[[nodiscard]] bool testRejectsEmptyKey() {
    const AsterKV::Wal::WalRecord record =
        AsterKV::Wal::makeSetRecord(1, "", "alex");

    return !AsterKV::Wal::validateWalRecord(record).isOk();
}

[[nodiscard]] bool testRejectsSetWithEmptyValue() {
    const AsterKV::Wal::WalRecord record =
        AsterKV::Wal::makeSetRecord(1, "username", "");

    return !AsterKV::Wal::validateWalRecord(record).isOk();
}

[[nodiscard]] bool testRejectsDelWithValue() {
    const AsterKV::Wal::WalRecord record{
        .sequenceNumber = 1,
        .type = AsterKV::Wal::WalRecordType::Del,
        .key = "username",
        .value = "unexpected",
    };

    return !AsterKV::Wal::validateWalRecord(record).isOk();
}

} // namespace

int main() {
    if (!testMakesSetRecord()) {
        return 1;
    }

    if (!testMakesDelRecord()) {
        return 1;
    }

    if (!testRecordTypeToString()) {
        return 1;
    }

    if (!testValidatesSetRecord()) {
        return 1;
    }

    if (!testValidatesDelRecord()) {
        return 1;
    }

    if (!testRejectsZeroSequenceNumber()) {
        return 1;
    }

    if (!testRejectsEmptyKey()) {
        return 1;
    }

    if (!testRejectsSetWithEmptyValue()) {
        return 1;
    }

    if (!testRejectsDelWithValue()) {
        return 1;
    }

    return 0;
}
