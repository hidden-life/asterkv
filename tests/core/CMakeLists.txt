add_executable(asterkv_core_tests
        status_result_test.cpp
)

target_link_libraries(asterkv_core_tests
        PRIVATE
            AsterKV::Core

            GTest::gtest_main
)

asterkvApplyCompilerOptions(asterkv_core_tests)

include(GoogleTest)

gtest_discover_tests(asterkv_core_tests)
