add_executable(asterkv_public_headers_test
        public_headers_test.cpp
)

target_link_libraries(asterkv_public_headers_test
        PRIVATE

        AsterKV::Core
        AsterKV::Command
        AsterKV::Protocol
        AsterKV::Storage
        AsterKV::Execution
)

asterkvApplyCompilerOptions(asterkv_public_headers_test)

add_test(
        NAME asterkv_public_headers_test
        COMMAND asterkv_public_headers_test
)
