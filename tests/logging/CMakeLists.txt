add_executable(asterkv_logging_tests
        logger_test.cpp
)

target_link_libraries(asterkv_logging_tests
        PRIVATE
            AsterKV::Logging
)

asterkvApplyCompilerOptions(asterkv_logging_tests)

add_test(
        NAME asterkv_logging_tests
        COMMAND asterkv_logging_tests
)
