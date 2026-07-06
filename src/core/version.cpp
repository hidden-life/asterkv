#include "asterkv/core/version.h"

namespace AsterKV::Core {
    Version version() noexcept {
        return Version {
            0,
            1,
            0,
        };
    }

    std::string_view versionString() noexcept {
        return "0.1.0";
    }

    std::string_view projectName() noexcept {
        return "AsterKV";
    }
}
