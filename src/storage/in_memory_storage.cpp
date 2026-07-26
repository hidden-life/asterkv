#include <asterkv/storage/in_memory_storage.h>

#include <utility>

namespace AsterKV::Storage {
    namespace {
        [[nodiscard]] bool isEmptyKey(std::string_view key) noexcept {
            return key.empty();
        }
    }

    Core::Status InMemoryStorage::set(std::string key, std::string value) {
        if (isEmptyKey(key)) {
            return Core::Status::invalidArgument("key must not be empty");
        }

        values_[std::move(key)] = std::move(value);

        return Core::Status::ok();
    }

    Core::Result<std::string> InMemoryStorage::get(std::string_view key) const {
        if (isEmptyKey(key)) {
            return Core::Result<std::string>::failure(
                Core::Status::invalidArgument("key must not be empty")
            );
        }

        const auto it = values_.find(std::string { key });
        if (it == values_.end()) {
            return Core::Result<std::string>::failure(
                Core::Status::notFound("key not found")
            );
        }

        return Core::Result<std::string>::success(it->second);
    }

    Core::Status InMemoryStorage::remove(std::string_view key) {
        if (isEmptyKey(key)) {
            return Core::Status::invalidArgument("key must not be empty");
        }

        const auto erasedCount = values_.erase(std::string { key });
        if (erasedCount == 0) {
            return Core::Status::notFound("key not found");
        }

        return Core::Status::ok();
    }

    Core::Result<bool> InMemoryStorage::exists(std::string_view key) const {
        if (isEmptyKey(key)) {
            return Core::Result<bool>::failure(Core::Status::invalidArgument("key must not be empty"));
        }

        return Core::Result<bool>::success(values_.contains(std::string { key }));
    }

    std::size_t InMemoryStorage::size() const noexcept {
        return values_.size();
    }

    void InMemoryStorage::clear() {
        values_.clear();
    }
}
