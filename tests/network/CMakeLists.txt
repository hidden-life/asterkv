add_executable(asterkv_network_tests
        tcp_endpoint_test.cpp
)

target_link_libraries(asterkv_network_tests
        PRIVATE
        AsterKV::Network
)

asterkvApplyCompilerOptions(asterkv_network_tests)

add_test(
        NAME asterkv_network_tests
        COMMAND asterkv_network_tests
)
