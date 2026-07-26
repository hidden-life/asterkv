add_executable(asterkv_storage_tests
        in_memory_storage_test.cpp
)

target_link_libraries(asterkv_storage_tests
        PRIVATE
            AsterKV::Storage
)

asterkvApplyCompilerOptions(asterkv_storage_tests)

add_test(
        NAME asterkv_storage_tests
        COMMAND asterkv_storage_tests
)
