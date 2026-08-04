add_executable(asterd
        main.cpp
)

target_link_libraries(asterd
        PRIVATE
        AsterKV::Core
        AsterKV::Pipeline
        AsterKV::Storage
        AsterKV::Network
        AsterKV::Server
        AsterKV::Config
        AsterKV::Logging
        AsterKV::Wal
)

asterkvApplyCompilerOptions(asterd)

if (BUILD_TESTING)
    add_test(
            NAME asterd_prints_version
            COMMAND asterd --version
    )

    add_test(
            NAME asterd_local_ping
            COMMAND asterd --local PING
    )

    set_tests_properties(asterd_local_ping
            PROPERTIES
                PASS_REGULAR_EXPRESSION "\\+PONG"
    )

    add_test(
            NAME asterd_local_unknown_command
            COMMAND asterd --local "NOPE key"
    )

    set_tests_properties(asterd_local_unknown_command
            PROPERTIES
                PASS_REGULAR_EXPRESSION "-ERR invalid_argument unknown command"
    )

    add_test(
            NAME asterd_local_stdin_pipeline
            COMMAND /bin/sh -c "printf 'SET username alex\nGET username\n' | \"$<TARGET_FILE:asterd>\" --local"
    )

    set_tests_properties(asterd_local_stdin_pipeline
            PROPERTIES
                PASS_REGULAR_EXPRESSION "alex"
    )

    add_test(
            NAME asterd_rejects_invalid_listen_address
            COMMAND /bin/sh -c "\"$<TARGET_FILE:asterd>\" --listen invalid 2>&1; test $? -ne 0"
    )

    set_tests_properties(asterd_rejects_invalid_listen_address
            PROPERTIES
                PASS_REGULAR_EXPRESSION "Invalid listen address"
    )

    add_test(
            NAME asterd_rejects_missing_listen_address
            COMMAND /bin/sh -c "\"$<TARGET_FILE:asterd>\" --listen 2>&1; test $? -ne 0"
    )

    set_tests_properties(asterd_rejects_missing_listen_address
            PROPERTIES
                PASS_REGULAR_EXPRESSION "--listen requires"
    )

    add_test(
            NAME asterd_rejects_invalid_max_clients
            COMMAND /bin/sh -c "\"$<TARGET_FILE:asterd>\" --listen 127.0.0.1:17799 --max-clients 0 2>&1; test $? -ne 0"
    )

    set_tests_properties(asterd_rejects_invalid_max_clients
            PROPERTIES
                PASS_REGULAR_EXPRESSION "Invalid max clients"
    )

    add_test(
            NAME asterd_rejects_invalid_idle_timeout
            COMMAND /bin/sh -c "\"$<TARGET_FILE:asterd>\" --listen 127.0.0.1:17799 --idle-timeout 0 2>&1; test $? -ne 0"
    )

    set_tests_properties(asterd_rejects_invalid_idle_timeout
            PROPERTIES
                PASS_REGULAR_EXPRESSION "Invalid idle timeout"
    )

    add_test(
            NAME asterd_rejects_invalid_log_level
            COMMAND /bin/sh -c "\"$<TARGET_FILE:asterd>\" --listen 127.0.0.1:17799 --log-level verbose 2>&1; test $? -ne 0"
    )

    set_tests_properties(asterd_rejects_invalid_log_level
            PROPERTIES
            PASS_REGULAR_EXPRESSION "Invalid log level"
    )

    add_test(
            NAME asterd_rejects_missing_config_file
            COMMAND /bin/sh -c "\"$<TARGET_FILE:asterd>\" --config /tmp/asterkv_missing_config_file.conf 2>&1; test $? -ne 0"
    )

    set_tests_properties(asterd_rejects_missing_config_file
            PROPERTIES
                PASS_REGULAR_EXPRESSION "Config error"
    )

    add_test(
            NAME asterd_rejects_missing_wal_file_argument
            COMMAND /bin/sh -c "\"$<TARGET_FILE:asterd>\" --listen 127.0.0.1:17799 --wal-file 2>&1; test $? -ne 0"
    )

    set_tests_properties(asterd_rejects_missing_wal_file_argument
            PROPERTIES
                PASS_REGULAR_EXPRESSION "--wal-file requires"
    )
endif ()
