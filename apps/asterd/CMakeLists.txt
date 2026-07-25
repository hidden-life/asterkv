add_executable(asterd
        main.cpp
)

target_link_libraries(asterd
        PRIVATE
        AsterKV::Core
        AsterKV::Pipeline
        AsterKV::Storage
        AsterKV::Network
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
endif ()
