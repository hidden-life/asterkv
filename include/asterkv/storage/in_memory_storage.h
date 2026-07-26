#ifndef ASTERKV_STORAGE_IN_MEMORY_STORAGE_H
#define ASTERKV_STORAGE_IN_MEMORY_STORAGE_H

#include <unordered_map>
#include <asterkv/storage/storage_engine.h>

namespace AsterKV::Storage {
    class InMemoryStorage final : public StorageEngine {
    public:
        [[nodiscard]] Core::Status set(std::string key, std::string value) override;
        [[nodiscard]] Core::Result<std::string> get(std::string_view key) const override;
        [[nodiscard]] Core::Status remove(std::string_view key) override;
        [[nodiscard]] Core::Result<bool> exists(std::string_view key) const override;

        [[nodiscard]] std::size_t size() const noexcept;
        void clear();

    private:
        std::unordered_map<std::string, std::string> values_;
    };
}

#endif //ASTERKV_STORAGE_IN_MEMORY_STORAGE_H
