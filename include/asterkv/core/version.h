#ifndef ASTERKV_CORE_VERSION_H
#define ASTERKV_CORE_VERSION_H

#include <string_view>

namespace AsterKV::Core {
    struct Version final {
        int major;
        int minor;
        int patch;
    };

    [[nodiscard]] Version version() noexcept;
    [[nodiscard]] std::string_view versionString() noexcept;
    [[nodiscard]] std::string_view projectName() noexcept;
}

#endif //ASTERKV_CORE_VERSION_H
