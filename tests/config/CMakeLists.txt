add_executable(asterkv_config_tests
        server_config_test.cpp
)

target_link_libraries(asterkv_config_tests
        PRIVATE
            AsterKV::Config
)

asterkvApplyCompilerOptions(asterkv_config_tests)

add_test(
        NAME asterkv_config_tests
        COMMAND asterkv_config_tests
)
