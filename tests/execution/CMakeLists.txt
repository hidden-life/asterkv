add_executable(asterkv_execution_tests
        command_dispatcher_test.cpp
)

target_link_libraries(asterkv_execution_tests
        PRIVATE
            AsterKV::Execution
)

asterkvApplyCompilerOptions(asterkv_execution_tests)

add_test(
        NAME asterkv_execution_tests
        COMMAND asterkv_execution_tests
)
