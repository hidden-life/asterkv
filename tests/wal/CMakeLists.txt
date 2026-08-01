add_executable(asterkv_wal_record_tests
        wal_record_test.cpp
)

target_link_libraries(asterkv_wal_record_tests
        PRIVATE
            AsterKV::Wal
)

asterkvApplyCompilerOptions(asterkv_wal_record_tests)

add_test(
        NAME asterkv_wal_record_tests
        COMMAND asterkv_wal_record_tests
)

add_executable(asterkv_wal_record_codec_tests
        wal_record_codec_test.cpp
)

target_link_libraries(asterkv_wal_record_codec_tests
        PRIVATE
        AsterKV::Wal
)

asterkvApplyCompilerOptions(asterkv_wal_record_codec_tests)

add_test(
        NAME asterkv_wal_record_codec_tests
        COMMAND asterkv_wal_record_codec_tests
)

add_executable(asterkv_wal_file_writer_tests
        wal_file_writer_test.cpp
)

target_link_libraries(asterkv_wal_file_writer_tests
        PRIVATE
        AsterKV::Wal
)

asterkvApplyCompilerOptions(asterkv_wal_file_writer_tests)

add_test(
        NAME asterkv_wal_file_writer_tests
        COMMAND asterkv_wal_file_writer_tests
)

add_executable(asterkv_wal_file_reader_tests
        wal_file_reader_test.cpp
)

target_link_libraries(asterkv_wal_file_reader_tests
        PRIVATE
        AsterKV::Wal
)

asterkvApplyCompilerOptions(asterkv_wal_file_reader_tests)

add_test(
        NAME asterkv_wal_file_reader_tests
        COMMAND asterkv_wal_file_reader_tests
)
