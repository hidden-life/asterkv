add_executable(asterkv_client_tests
        response_renderer_test.cpp
)

target_link_libraries(asterkv_client_tests
        PRIVATE
        AsterKV::Client
)

asterkvApplyCompilerOptions(asterkv_client_tests)

add_test(
        NAME asterkv_client_tests
        COMMAND asterkv_client_tests
)
