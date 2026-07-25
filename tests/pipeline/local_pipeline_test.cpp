#include <asterkv/pipeline/local_pipeline.h>
#include <asterkv/storage/in_memory_storage.h>

#include <string>

namespace {

[[nodiscard]] bool expectEqual(std::string_view left, std::string_view right) {
    return left == right;
}

[[nodiscard]] bool testProcessesPing() {
    AsterKV::Storage::InMemoryStorage storage;
    AsterKV::Pipeline::LocalPipeline pipeline{storage};

    return expectEqual(pipeline.processLine("PING"), "+PONG\r\n");
}

[[nodiscard]] bool testProcessesSetAndGet() {
    AsterKV::Storage::InMemoryStorage storage;
    AsterKV::Pipeline::LocalPipeline pipeline{storage};

    const std::string setResponse = pipeline.processLine("SET username jackson");
    const std::string getResponse = pipeline.processLine("GET username");

    return expectEqual(setResponse, "+OK\r\n") &&
           expectEqual(getResponse, "$7\r\njackson\r\n");
}

[[nodiscard]] bool testProcessesExistsForExistingKey() {
    AsterKV::Storage::InMemoryStorage storage;
    AsterKV::Pipeline::LocalPipeline pipeline{storage};

    if (!expectEqual(pipeline.processLine("SET key value"), "+OK\r\n")) {
        return false;
    }

    return expectEqual(pipeline.processLine("EXISTS key"), ":1\r\n");
}

[[nodiscard]] bool testProcessesExistsForMissingKey() {
    AsterKV::Storage::InMemoryStorage storage;
    AsterKV::Pipeline::LocalPipeline pipeline{storage};

    return expectEqual(pipeline.processLine("EXISTS missing"), ":0\r\n");
}

[[nodiscard]] bool testProcessesDeleteExistingKey() {
    AsterKV::Storage::InMemoryStorage storage;
    AsterKV::Pipeline::LocalPipeline pipeline{storage};

    if (!expectEqual(pipeline.processLine("SET key value"), "+OK\r\n")) {
        return false;
    }

    const std::string delResponse = pipeline.processLine("DEL key");
    const std::string existsResponse = pipeline.processLine("EXISTS key");

    return expectEqual(delResponse, ":1\r\n") &&
           expectEqual(existsResponse, ":0\r\n");
}

[[nodiscard]] bool testProcessesDeleteMissingKey() {
    AsterKV::Storage::InMemoryStorage storage;
    AsterKV::Pipeline::LocalPipeline pipeline{storage};

    return expectEqual(pipeline.processLine("DEL missing"), ":0\r\n");
}

[[nodiscard]] bool testReturnsErrorForUnknownCommand() {
    AsterKV::Storage::InMemoryStorage storage;
    AsterKV::Pipeline::LocalPipeline pipeline{storage};

    return expectEqual(pipeline.processLine("NOPE key"),
                       "-ERR invalid_argument unknown command\r\n");
}

[[nodiscard]] bool testReturnsErrorForInvalidArgumentCount() {
    AsterKV::Storage::InMemoryStorage storage;
    AsterKV::Pipeline::LocalPipeline pipeline{storage};

    return expectEqual(pipeline.processLine("GET"),
                       "-ERR invalid_argument invalid command argument count\r\n");
}

[[nodiscard]] bool testReturnsErrorForMissingKey() {
    AsterKV::Storage::InMemoryStorage storage;
    AsterKV::Pipeline::LocalPipeline pipeline{storage};

    return expectEqual(pipeline.processLine("GET missing"),
                       "-ERR not_found key not found\r\n");
}

} // namespace

int main() {
    if (!testProcessesPing()) {
        return 1;
    }

    if (!testProcessesSetAndGet()) {
        return 1;
    }

    if (!testProcessesExistsForExistingKey()) {
        return 1;
    }

    if (!testProcessesExistsForMissingKey()) {
        return 1;
    }

    if (!testProcessesDeleteExistingKey()) {
        return 1;
    }

    if (!testProcessesDeleteMissingKey()) {
        return 1;
    }

    if (!testReturnsErrorForUnknownCommand()) {
        return 1;
    }

    if (!testReturnsErrorForInvalidArgumentCount()) {
        return 1;
    }

    if (!testReturnsErrorForMissingKey()) {
        return 1;
    }

    return 0;
}
