#include <asterkv/pipeline/local_pipeline.h>
#include <asterkv/protocol/parser.h>
#include <asterkv/protocol/response_serializer.h>

namespace AsterKV::Pipeline {
    LocalPipeline::LocalPipeline(Storage::StorageEngine &storage) : dispatcher_(storage) {
    }

    std::string LocalPipeline::processLine(std::string_view line) {
        auto commandRequest = Protocol::parseCommandLine(line);
        if (commandRequest.isError()) {
            return Protocol::serializeStatus(commandRequest.status());
        }

        auto executionResult = dispatcher_.dispatch(commandRequest.value());

        return Protocol::serializeExecutionResult(executionResult);
    }
}
