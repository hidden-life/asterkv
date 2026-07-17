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

add_executable(asterkv_protocol_response_tests
        response_serializer_test.cpp
)

target_link_libraries(asterkv_protocol_response_tests
        PRIVATE
        AsterKV::Protocol
        AsterKV::Execution
)

asterkvApplyCompilerOptions(asterkv_protocol_response_tests)

add_test(
        NAME asterkv_protocol_response_tests
        COMMAND asterkv_protocol_response_tests
)
