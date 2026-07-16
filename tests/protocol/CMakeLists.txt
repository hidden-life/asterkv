add_executable(asterkv_protocol_tests
        protocol_parser_test.cpp
)

target_link_libraries(asterkv_protocol_tests
        PRIVATE
            AsterKV::Protocol
)

asterkvApplyCompilerOptions(asterkv_protocol_tests)

add_test(
        NAME asterkv_protocol_tests
        COMMAND asterkv_protocol_tests
)
