#include <filesystem>
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

        [[nodiscard]] Core::Status validateParentDirectory(std::string_view path) {
            const std::filesystem::path p {std::string {path}};
            const std::filesystem::path pp = p.parent_path();

            if (pp.empty()) {
                return Core::Status::ok();
            }

            std::error_code error;
            if (!std::filesystem::exists(pp, error)) {
                return Core::Status::unavailable("WAL parent directory does not exists");
            }

            if (error) {
                return Core::Status::unavailable("failed to inspect WAL parent directory");
            }

            if (!std::filesystem::is_directory(pp, error)) {
                return Core::Status::unavailable("WAL parent path is not a directory");
            }

            if (error) {
                return Core::Status::unavailable("failed to inspect WAL parent directory type");
            }

            return Core::Status::ok();
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
        nextSequenceNumber_(1),
        writer_(filePath_, options_.writerOptions) {
    }

    const std::string & WalBackedStorage::filePath() const noexcept {
        return filePath_;
    }

    WalSequenceNumber WalBackedStorage::nextSequenceNumber() const noexcept {
        std::lock_guard lock {mutex_};

        return nextSequenceNumber_;
    }

    Core::Status WalBackedStorage::recover() {
        std::lock_guard lock {mutex_};

        const Core::Status pathStatus = validateWalFilepathForRecovery();
        if (!pathStatus.isOk()) {
            return pathStatus;
        }

        std::error_code exitsError;
        const bool fileExists = std::filesystem::exists(filePath_, exitsError);
        if (exitsError) {
            return Core::Status::unavailable("failed to inspect WAL file");
        }

        if (!fileExists) {
            nextSequenceNumber_ = 1;

            return writer_.open();
        }

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

        return writer_.open();
    }

    Core::Status WalBackedStorage::set(std::string key, std::string value) {
        std::lock_guard lock {mutex_};

        const WalRecord record = makeSetRecord(nextSequenceNumber_, key, value);
        const Core::Status validationStatus = validateWalRecord(record);
        if (!validationStatus.isOk()) {
            return validationStatus;
        }

        const Core::Status appendStatus = writer_.appendRecord(record);
        if (!appendStatus.isOk()) {
            return appendStatus;
        }

        const Core::Status storageStatus = storage_.set(std::move(key), std::move(value));
        if (!storageStatus.isOk()) {
            return storageStatus;
        }

        ++nextSequenceNumber_;

        return Core::Status::ok();
    }

    Core::Result<std::string> WalBackedStorage::get(std::string_view key) const {
        return storage_.get(key);
    }

    Core::Status WalBackedStorage::remove(std::string_view key) {
        std::lock_guard lock {mutex_};

        Core::Result<bool> existsResult = storage_.exists(key);
        if (existsResult.isError()) {
            return existsResult.status();
        }

        if (!existsResult.value()) {
            return storage_.remove(key);
        }

        const WalRecord record = makeDelRecord(nextSequenceNumber_, std::string {key});
        const Core::Status validationStatus = validateWalRecord(record);
        if (!validationStatus.isOk()) {
            return validationStatus;
        }

        const Core::Status appendStatus = writer_.appendRecord(record);
        if (!appendStatus.isOk()) {
            return appendStatus;
        }

        const Core::Status storageStatus = storage_.remove(key);
        if (!storageStatus.isOk()) {
            return storageStatus;
        }

        ++nextSequenceNumber_;

        return Core::Status::ok();
    }

    Core::Result<bool> WalBackedStorage::exists(std::string_view key) const {
        return storage_.exists(key);
    }

    Core::Status WalBackedStorage::del(std::string key) {
        return remove(key);
    }

    void WalBackedStorage::advanceSequenceNumber() noexcept {
        ++nextSequenceNumber_;
    }

    Core::Status WalBackedStorage::validateWalFilepathForRecovery() const {
        if (filePath_.empty()) {
            return Core::Status::invalidArgument("WAL file path must not be empty");
        }

        return validateParentDirectory(filePath_);
    }
}
