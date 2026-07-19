add_executable(asterkv_pipeline_tests
        local_pipeline_test.cpp
)

target_link_libraries(asterkv_pipeline_tests
        PRIVATE
        AsterKV::Pipeline
)

asterkvApplyCompilerOptions(asterkv_pipeline_tests)

add_test(
        NAME asterkv_pipeline_tests
        COMMAND asterkv_pipeline_tests
)
