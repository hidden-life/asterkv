#include <asterkv/wal/wal_record_codec.h>

#include <string>

namespace {

[[nodiscard]] bool testSerializesSetRecord() {
    const AsterKV::Wal::WalRecord record =
        AsterKV::Wal::makeSetRecord(1, "username", "jackson");

    const auto serialized = AsterKV::Wal::serialize(record);

    return serialized.isOk() &&
           serialized.value() ==
               "AKVWAL1 1 set 757365726e616d65 6a61636b736f6e\n";
}

[[nodiscard]] bool testSerializesDelRecord() {
    const AsterKV::Wal::WalRecord record =
        AsterKV::Wal::makeDelRecord(2, "username");

    const auto serialized = AsterKV::Wal::serialize(record);

    return serialized.isOk() &&
           serialized.value() ==
               "AKVWAL1 2 del 757365726e616d65 -\n";
}

[[nodiscard]] bool testDeserializesSetRecord() {
    const auto record = AsterKV::Wal::deserialize(
        "AKVWAL1 1 set 757365726e616d65 6a61636b736f6e\n");

    return record.isOk() &&
           record.value().sequenceNumber == 1 &&
           record.value().type == AsterKV::Wal::WalRecordType::Set &&
           record.value().key == "username" &&
           record.value().value == "jackson";
}

[[nodiscard]] bool testDeserializesDelRecord() {
    const auto record = AsterKV::Wal::deserialize(
        "AKVWAL1 2 del 757365726e616d65 -\n");

    return record.isOk() &&
           record.value().sequenceNumber == 2 &&
           record.value().type == AsterKV::Wal::WalRecordType::Del &&
           record.value().key == "username" &&
           record.value().value.empty();
}

[[nodiscard]] bool testDeserializesCrLfRecord() {
    const auto record = AsterKV::Wal::deserialize(
        "AKVWAL1 3 set 6b6579 76616c7565\r\n");

    return record.isOk() &&
           record.value().sequenceNumber == 3 &&
           record.value().type == AsterKV::Wal::WalRecordType::Set &&
           record.value().key == "key" &&
           record.value().value == "value";
}

[[nodiscard]] bool testRoundTripsRecordWithSpaces() {
    const AsterKV::Wal::WalRecord original =
        AsterKV::Wal::makeSetRecord(4, "user name", "jack son");

    const auto serialized = AsterKV::Wal::serialize(original);

    if (serialized.isError()) {
        return false;
    }

    const auto restored = AsterKV::Wal::deserialize(serialized.value());

    return restored.isOk() &&
           restored.value().sequenceNumber == original.sequenceNumber &&
           restored.value().type == original.type &&
           restored.value().key == original.key &&
           restored.value().value == original.value;
}

[[nodiscard]] bool testRoundTripsRecordWithSymbols() {
    const AsterKV::Wal::WalRecord original =
        AsterKV::Wal::makeSetRecord(5, "user:name", "value=one/two");

    const auto serialized = AsterKV::Wal::serialize(original);

    if (serialized.isError()) {
        return false;
    }

    const auto restored = AsterKV::Wal::deserialize(serialized.value());

    return restored.isOk() &&
           restored.value().sequenceNumber == original.sequenceNumber &&
           restored.value().type == original.type &&
           restored.value().key == original.key &&
           restored.value().value == original.value;
}

[[nodiscard]] bool testRejectsInvalidRecordForSerialization() {
    const AsterKV::Wal::WalRecord invalidRecord =
        AsterKV::Wal::makeSetRecord(0, "username", "jackson");

    return AsterKV::Wal::serialize(invalidRecord).isError();
}

[[nodiscard]] bool testRejectsEmptySerializedRecord() {
    return AsterKV::Wal::deserialize("").isError();
}

[[nodiscard]] bool testRejectsUnsupportedVersion() {
    return AsterKV::Wal::deserialize(
        "BADWAL1 1 set 6b6579 76616c7565\n").isError();
}

[[nodiscard]] bool testRejectsInvalidFieldCount() {
    return AsterKV::Wal::deserialize(
        "AKVWAL1 1 set 6b6579\n").isError();
}

[[nodiscard]] bool testRejectsEmptyFieldFromDoubleSpace() {
    return AsterKV::Wal::deserialize(
        "AKVWAL1 1  set 6b6579 76616c7565\n").isError();
}

[[nodiscard]] bool testRejectsInvalidSequenceNumber() {
    return AsterKV::Wal::deserialize(
        "AKVWAL1 nope set 6b6579 76616c7565\n").isError();
}

[[nodiscard]] bool testRejectsZeroSequenceNumber() {
    return AsterKV::Wal::deserialize(
        "AKVWAL1 0 set 6b6579 76616c7565\n").isError();
}

[[nodiscard]] bool testRejectsUnknownType() {
    return AsterKV::Wal::deserialize(
        "AKVWAL1 1 put 6b6579 76616c7565\n").isError();
}

[[nodiscard]] bool testRejectsOddHexLength() {
    return AsterKV::Wal::deserialize(
        "AKVWAL1 1 set 6b657 76616c7565\n").isError();
}

[[nodiscard]] bool testRejectsInvalidHexDigit() {
    return AsterKV::Wal::deserialize(
        "AKVWAL1 1 set 6b6579 zz\n").isError();
}

[[nodiscard]] bool testRejectsEmptyKey() {
    return AsterKV::Wal::deserialize(
        "AKVWAL1 1 set - 76616c7565\n").isError();
}

[[nodiscard]] bool testRejectsSetWithEmptyValue() {
    return AsterKV::Wal::deserialize(
        "AKVWAL1 1 set 6b6579 -\n").isError();
}

[[nodiscard]] bool testRejectsDelWithValue() {
    return AsterKV::Wal::deserialize(
        "AKVWAL1 1 del 6b6579 76616c7565\n").isError();
}

} // namespace

int main() {
    if (!testSerializesSetRecord()) {
        return 1;
    }

    if (!testSerializesDelRecord()) {
        return 1;
    }

    if (!testDeserializesSetRecord()) {
        return 1;
    }

    if (!testDeserializesDelRecord()) {
        return 1;
    }

    if (!testDeserializesCrLfRecord()) {
        return 1;
    }

    if (!testRoundTripsRecordWithSpaces()) {
        return 1;
    }

    if (!testRoundTripsRecordWithSymbols()) {
        return 1;
    }

    if (!testRejectsInvalidRecordForSerialization()) {
        return 1;
    }

    if (!testRejectsEmptySerializedRecord()) {
        return 1;
    }

    if (!testRejectsUnsupportedVersion()) {
        return 1;
    }

    if (!testRejectsInvalidFieldCount()) {
        return 1;
    }

    if (!testRejectsEmptyFieldFromDoubleSpace()) {
        return 1;
    }

    if (!testRejectsInvalidSequenceNumber()) {
        return 1;
    }

    if (!testRejectsZeroSequenceNumber()) {
        return 1;
    }

    if (!testRejectsUnknownType()) {
        return 1;
    }

    if (!testRejectsOddHexLength()) {
        return 1;
    }

    if (!testRejectsInvalidHexDigit()) {
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
