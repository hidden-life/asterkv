#ifndef ASTERKV_PIPELINE_LOCAL_PIPELINE_H
#define ASTERKV_PIPELINE_LOCAL_PIPELINE_H

#include <asterkv/storage/storage_engine.h>
#include <asterkv/execution/command_dispatcher.h>

#include <string>
#include <string_view>

namespace AsterKV::Pipeline {
    class LocalPipeline final {
    public:
        explicit LocalPipeline(Storage::StorageEngine &storage);

        [[nodiscard]] std::string processLine(std::string_view line);

    private:
        Execution::CommandDispatcher dispatcher_;
    };
}

#endif //ASTERKV_PIPELINE_LOCAL_PIPELINE_H
