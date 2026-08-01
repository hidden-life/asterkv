#include <asterkv/wal/wal_record_codec.h>

#include <array>
#include <limits>
#include <vector>

namespace AsterKV::Wal {
    namespace {
        constexpr std::string_view emptyWalFieldMarker = "-";

        [[nodiscard]] char toHex(std::uint8_t value) noexcept {
            constexpr std::array<char, 16> digits {
                '0', '1', '2', '3',
                '4', '5', '6', '7',
                '8', '9', 'a', 'b',
                'c', 'd', 'e', 'f',
            };

            return digits[value & 0x0F];
        }

        [[nodiscard]] bool isHexDigit(char value) noexcept {
            return (value >= '0' && value <= '9') || (value >= 'a' && value <= 'f') || (value >= 'A' && value <= 'F');
        }

        [[nodiscard]] Core::Result<std::uint8_t> fromHex(char value) {
            if (value >= '0' && value <= '9') {
                return Core::Result<std::uint8_t>::success(
                    static_cast<std::uint8_t>(value - '0')
                );
            }

            if (value >= 'a' && value <= 'f') {
                return Core::Result<std::uint8_t>::success(
                    static_cast<std::uint8_t>(10 + value - 'a')
                );
            }

            if (value >= 'A' && value <= 'F') {
                return Core::Result<std::uint8_t>::success(
                    static_cast<std::uint8_t>(10 + value - 'A')
                );
            }

            return Core::Result<std::uint8_t>::failure(Core::Status::invalidArgument("invalid hex digit"));
        }

        [[nodiscard]] std::string hexEncode(std::string_view value) {
            std::string out;
            out.reserve(value.size() * 2);

            for (const char ch : value) {
                const auto byte = static_cast<std::uint8_t>(static_cast<unsigned char>(ch));

                out.push_back(toHex(static_cast<std::uint8_t>(byte >> 4)));
                out.push_back(toHex(static_cast<std::uint8_t>(byte)));
            }

            return out;
        }

        [[nodiscard]] Core::Result<std::string> hexDecode(std::string_view value) {
            if (value == emptyWalFieldMarker) {
                return Core::Result<std::string>::success({});
            }

            if (value.empty()) {
                return Core::Result<std::string>::failure(Core::Status::invalidArgument("hex field must not be empty"));
            }

            if ((value.size() % 2) != 0) {
                return Core::Result<std::string>::failure(
                    Core::Status::invalidArgument("hex field has odd length")
                );
            }

            std::string out;
            out.reserve(value.size() / 2);

            for (std::size_t index = 0; index < value.size(); index += 2) {
                if (!isHexDigit(value[index]) || !isHexDigit(value[index + 1])) {
                    return Core::Result<std::string>::failure(Core::Status::invalidArgument("hex field contains invalid digit"));
                }

                Core::Result<std::uint8_t> high = fromHex(value[index]);
                if (high.isError()) {
                    return Core::Result<std::string>::failure(high.status());
                }

                Core::Result<std::uint8_t> low = fromHex(value[index + 1]);
                if (low.isError()) {
                    return Core::Result<std::string>::failure(low.status());
                }

                const auto byte = static_cast<char>(static_cast<std::uint8_t>((high.value() << 4U) | low.value()));
                out.push_back(byte);
            }

            return Core::Result<std::string>::success(out);
        }

        [[nodiscard]] std::string encodeWalField(std::string_view value) {
            if (value.empty()) {
                return std::string {emptyWalFieldMarker};
            }

            return hexEncode(value);
        }

        [[nodiscard]] bool isDigit(char value) noexcept {
            return value >= '0' && value <= '9';
        }

        [[nodiscard]] Core::Result<WalSequenceNumber> parseSequenceNumber(std::string_view value) {
            if (value.empty()) {
                return Core::Result<WalSequenceNumber>::failure(
                    Core::Status::invalidArgument("WAL sequence number is empty")
                );
            }

            WalSequenceNumber result = 0;

            for (const char ch : value) {
                if (!isDigit(ch)) {
                    return Core::Result<WalSequenceNumber>::failure(
                        Core::Status::invalidArgument("WAL sequence number is not numeric")
                    );
                }

                const WalSequenceNumber digit = static_cast<WalSequenceNumber>(ch - '0');

                if (result > (std::numeric_limits<WalSequenceNumber>::max() - digit) / 10U) {
                    return Core::Result<WalSequenceNumber>::failure(
                        Core::Status::invalidArgument("WAL sequence number is too large")
                    );
                }

                result = result * 10U + digit;
            }

            return Core::Result<WalSequenceNumber>::success(result);
        }

        [[nodiscard]] Core::Result<WalRecordType> parseRecordType(std::string_view value) {
            if (value == "set") {
                return Core::Result<WalRecordType>::success(WalRecordType::Set);
            }

            if (value == "del") {
                return Core::Result<WalRecordType>::success(WalRecordType::Del);
            }

            return Core::Result<WalRecordType>::failure(Core::Status::invalidArgument("unknown WAL record type"));
        }

        [[nodiscard]] std::string_view stripLineEnding(std::string_view value) noexcept {
            if (!value.empty() && value.back() == '\n') {
                value.remove_suffix(1);
            }

            if (!value.empty() && value.back() == '\r') {
                value.remove_suffix(1);
            }

            return value;
        }

        [[nodiscard]] std::vector<std::string_view> splitBySingleSpaces(std::string_view value) {
            std::vector<std::string_view> parts;
            std::size_t start = 0;

            while (start <= value.size()) {
                const std::size_t separator = value.find(' ', start);
                if (separator == std::string_view::npos) {
                    parts.push_back(value.substr(start));
                    break;
                }

                parts.push_back(value.substr(start, separator - start));
                start = separator + 1;
            }

            return parts;
        }

        [[nodiscard]] bool hasEmptyPart(const std::vector<std::string_view> &parts) {
            for (const std::string_view part : parts) {
                if (part.empty()) {
                    return true;
                }
            }

            return false;
        }
    }

    Core::Result<std::string> serialize(const WalRecord &record) {
        const Core::Status validationStatus = validateWalRecord(record);
        if (!validationStatus.isOk()) {
            return Core::Result<std::string>::failure(validationStatus);
        }

        std::string out;
        out.reserve(record.key.size() * 2 + record.value.size() * 2 + 64);

        out.append(currentWalRecordFormatVersion);
        out.push_back(' ');
        out.append(std::to_string(record.sequenceNumber));
        out.push_back(' ');
        out.append(walRecordTypeToString(record.type));
        out.push_back(' ');
        out.append(encodeWalField(record.key));
        out.push_back(' ');
        out.append(encodeWalField(record.value));
        out.push_back('\n');

        return Core::Result<std::string>::success(out);
    }

    Core::Result<WalRecord> deserialize(std::string_view serialized) {
        serialized = stripLineEnding(serialized);
        if (serialized.empty()) {
            return Core::Result<WalRecord>::failure(Core::Status::invalidArgument("serialized WAL record is empty"));
        }

        const std::vector<std::string_view> parts = splitBySingleSpaces(serialized);
        if (parts.size() != 5 || hasEmptyPart(parts)) {
            return Core::Result<WalRecord>::failure(Core::Status::invalidArgument("serialized WAL record must contain 5 fields"));
        }

        if (parts[0] != currentWalRecordFormatVersion) {
            return Core::Result<WalRecord>::failure(Core::Status::invalidArgument("unsupported WAL record format version"));
        }

        Core::Result<WalSequenceNumber> seqNumber = parseSequenceNumber(parts[1]);
        if (seqNumber.isError()) {
            return Core::Result<WalRecord>::failure(seqNumber.status());
        }

        Core::Result<WalRecordType> recordType = parseRecordType(parts[2]);
        if (recordType.isError()) {
            return Core::Result<WalRecord>::failure(recordType.status());
        }

        Core::Result<std::string> key = hexDecode(parts[3]);
        if (key.isError()) {
            return Core::Result<WalRecord>::failure(key.status());
        }

        Core::Result<std::string> value = hexDecode(parts[4]);
        if (value.isError()) {
            return Core::Result<WalRecord>::failure(value.status());
        }

        WalRecord record {
            .sequenceNumber = seqNumber.value(),
            .type = recordType.value(),
            .key = key.value(),
            .value = value.value()
        };

        const Core::Status validationStatus = validateWalRecord(record);
        if (!validationStatus.isOk()) {
            return Core::Result<WalRecord>::failure(validationStatus);
        }

        return Core::Result<WalRecord>::success(record);
    }
}
