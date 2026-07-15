#include <asterkv/core/result.h>
#include <asterkv/core/status.h>
#include <asterkv/core/version.h>

#include <string>

int main() {
    auto result = AsterKV::Core::Result<std::string>::success("value");
    if (!result.isOk()) {
        return 1;
    }

    const AsterKV::Core::Status status = AsterKV::Core::Status::ok();

    if (!status.isOk()) {
        return 1;
    }

    const AsterKV::Core::Version currentVersion = AsterKV::Core::version();
    if (currentVersion.major != 0) {
        return 1;
    }

    return 0;
}
