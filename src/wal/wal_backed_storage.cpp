#include <asterkv/wal/wal_backed_storage.h>
#include <asterkv/wal/wal_file_reader.h>

#include <vector>

namespace AsterKV::Wal {
    namespace {
        [[nodiscard]] WalSequenceNumber computeNextSequenceNumber(const std::vector<WalRecord> &records) noexcept {
            WalSequenceNumber maxSequenceNumber = 0;

            for (const WalRecord &record : records) {
                if (record.sequenceNumber > maxSequenceNumber) {
                    maxSequenceNumber = record.sequenceNumber;
                }
            }

            return maxSequenceNumber + 1;
        }
    }

    InMemoryWalReplayTarget::InMemoryWalReplayTarget(Storage::InMemoryStorage &storage) : storage_(storage) {
    }

    Core::Status InMemoryWalReplayTarget::set(std::string_view key, std::string_view value) {
        return storage_.set(std::string {key}, std::string {value});
    }

    Core::Status InMemoryWalReplayTarget::del(std::string_view value) {
        return storage_.remove(std::string {value});
    }

    WalBackedStorage::WalBackedStorage(
        Storage::InMemoryStorage &storage,
        std::string filePath,
        WalBackedStorageOptions options) :
            storage_(storage),
            filePath_(std::move(filePath)),
            options_(options),
            nextSequenceNumber_(1) {
    }

    const std::string & WalBackedStorage::filePath() const noexcept {
        return filePath_;
    }

    WalSequenceNumber WalBackedStorage::nextSequenceNumber() const noexcept {
        return nextSequenceNumber_;
    }

    Core::Status WalBackedStorage::recover() {
        Core::Result<std::vector<WalRecord>> records = readRecordsFromFile(filePath_);
        if (records.isError()) {
            return records.status();
        }

        InMemoryWalReplayTarget target {storage_};
        const Core::Status replayStatus = replayRecords(records.value(), target);
        if (!replayStatus.isOk()) {
            return replayStatus;
        }

        nextSequenceNumber_ = computeNextSequenceNumber(records.value());

        return Core::Status::ok();
    }

    Core::Status WalBackedStorage::set(std::string key, std::string value) {
        const WalSequenceNumber seqNumber = allocateSequenceNumber();
        const WalRecord record = makeSetRecord(seqNumber, key, value);
        const Core::Status appendStatus = appendWalRecordToFile(filePath_, record, options_.writerOptions);
        if (!appendStatus.isOk()) {
            return appendStatus;
        }

        return storage_.set(std::move(key), std::move(value));
    }

    Core::Status WalBackedStorage::del(std::string key) {
        const WalSequenceNumber seqNumber = allocateSequenceNumber();
        const WalRecord record = makeDelRecord(seqNumber, key);
        const Core::Status appendStatus = appendWalRecordToFile(filePath_, record, options_.writerOptions);
        if (!appendStatus.isOk()) {
            return appendStatus;
        }

        return storage_.remove(std::move(key));
    }

    WalSequenceNumber WalBackedStorage::allocateSequenceNumber() noexcept {
        const WalSequenceNumber allocated = nextSequenceNumber_;
        ++nextSequenceNumber_;

        return allocated;
    }
}
