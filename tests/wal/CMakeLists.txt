add_executable(asterkv_wal_tests
        wal_record_test.cpp
)

target_link_libraries(asterkv_wal_tests
        PRIVATE
            AsterKV::Wal
)

asterkvApplyCompilerOptions(asterkv_wal_tests)

add_test(
        NAME asterkv_wal_tests
        COMMAND asterkv_wal_tests
)
