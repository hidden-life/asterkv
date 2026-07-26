#ifndef ASTERKV_STORAGE_STORAGE_ENGINE_H
#define ASTERKV_STORAGE_STORAGE_ENGINE_H

#include <asterkv/core/result.h>
#include <asterkv/core/status.h>

namespace AsterKV::Storage {
    class StorageEngine {
    public:
        virtual ~StorageEngine() = default;

        [[nodiscard]] virtual Core::Status set(std::string key, std::string value) = 0;
        [[nodiscard]] virtual Core::Result<std::string> get(std::string_view key) const = 0;
        [[nodiscard]] virtual Core::Status remove(std::string_view key) = 0;
        [[nodiscard]] virtual Core::Result<bool> exists(std::string_view key) const = 0;
    };
}

#endif //ASTERKV_STORAGE_STORAGE_ENGINE_H
