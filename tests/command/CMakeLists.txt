add_executable(asterkv_command_tests
    command_model_test.cpp
)

target_link_libraries(asterkv_command_tests PRIVATE AsterKV::Command)

asterkvApplyCompilerOptions(asterkv_command_tests)

add_test(
        NAME asterkv_command_tests
        COMMAND asterkv_command_tests
)
