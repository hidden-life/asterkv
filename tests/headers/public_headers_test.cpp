#include <asterkv/core/result.h>
#include <asterkv/core/status.h>
#include <asterkv/core/version.h>
#include <asterkv/command/command.h>
#include <asterkv/protocol/parser.h>
#include <asterkv/storage/in_memory_storage.h>
#include <asterkv/storage/storage_engine.h>

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

    auto commandType = AsterKV::Command::commandTypeFromString("PING");
    if (!commandType.isOk()) {
        return 1;
    }

    auto commandRequest = AsterKV::Protocol::parseCommandLine("PING");
    if (!commandRequest.isOk()) {
        return 1;
    }

    AsterKV::Storage::InMemoryStorage storage;
    if (!storage.set("key", "value").isOk()) {
        return 1;
    }

    auto storageValue = storage.get("key");
    if (!storageValue.isOk()) {
        return 1;
    }

    return storageValue.value() == "value" ? 0 : 1;
}
