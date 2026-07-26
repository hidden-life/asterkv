add_executable(asterkv_server_tests
        tcp_server_runtime_test.cpp
)

target_link_libraries(asterkv_server_tests
        PRIVATE
            AsterKV::Server
)

asterkvApplyCompilerOptions(asterkv_server_tests)

add_test(
        NAME asterkv_server_tests
        COMMAND asterkv_server_tests
)
