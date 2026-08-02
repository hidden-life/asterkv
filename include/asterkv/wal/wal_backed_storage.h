#ifndef ASTERKV_WAL_WAL_BACKED_STORAGE_H
#define ASTERKV_WAL_WAL_BACKED_STORAGE_H

#include <asterkv/wal/wal_replay.h>
#include <asterkv/storage/in_memory_storage.h>
#include <asterkv/wal/wal_file_writer.h>
#include <asterkv/core/status.h>

#include <string>

namespace AsterKV::Wal {
    class InMemoryWalReplayTarget final : public WalReplayTarget {
    public:
        explicit InMemoryWalReplayTarget(Storage::InMemoryStorage &storage);

        [[nodiscard]] Core::Status set(std::string_view key, std::string_view value) override;

        [[nodiscard]] Core::Status del(std::string_view value) override;

    private:
        Storage::InMemoryStorage &storage_;
    };

    struct WalBackedStorageOptions final {
        WalFileWriterOptions writerOptions {};
    };

    class WalBackedStorage final {
    public:
        WalBackedStorage(Storage::InMemoryStorage &storage, std::string filePath, WalBackedStorageOptions options = {});

        [[nodiscard]] const std::string &filePath() const noexcept;
        [[nodiscard]] WalSequenceNumber nextSequenceNumber() const noexcept;

        [[nodiscard]] Core::Status recover();

        [[nodiscard]] Core::Status set(std::string key, std::string value);
        [[nodiscard]] Core::Status del(std::string key);

    private:
        [[nodiscard]] WalSequenceNumber allocateSequenceNumber() noexcept;

        Storage::InMemoryStorage &storage_;
        std::string filePath_;
        WalBackedStorageOptions options_;
        WalSequenceNumber nextSequenceNumber_;
    };
}

#endif //ASTERKV_WAL_WAL_BACKED_STORAGE_H
