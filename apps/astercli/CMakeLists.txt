add_executable(astercli
        main.cpp
)

target_link_libraries(astercli
        PRIVATE
        AsterKV::Core
        AsterKV::Pipeline
        AsterKV::Storage
        AsterKV::Network
        AsterKV::Client
)

asterkvApplyCompilerOptions(astercli)

if (BUILD_TESTING)
    add_test(
            NAME astercli_prints_version
            COMMAND astercli --version
    )

    add_test(
            NAME astercli_local_ping
            COMMAND astercli --local PING
    )

    set_tests_properties(
            astercli_local_ping
            PROPERTIES
                PASS_REGULAR_EXPRESSION "PONG"
    )

    add_test(
            NAME astercli_local_set
            COMMAND astercli --local "SET username alex"
    )

    set_tests_properties(
            astercli_local_set
            PROPERTIES
                PASS_REGULAR_EXPRESSION "OK"
    )

    add_test(
            NAME astercli_local_unknown_command
            COMMAND astercli --local "NOPE key"
    )

    set_tests_properties(
            astercli_local_unknown_command
            PROPERTIES
                PASS_REGULAR_EXPRESSION "error: invalid_argument unknown command"
    )

    add_test(
            NAME astercli_rejects_missing_connect_endpoint
            COMMAND /bin/sh -c "\"$<TARGET_FILE:astercli>\" --connect 2>&1; test $? -ne 0"
    )

    set_tests_properties(astercli_rejects_missing_connect_endpoint
            PROPERTIES
                PASS_REGULAR_EXPRESSION "--connect requires"
    )

    add_test(
            NAME astercli_rejects_invalid_connect_endpoint
            COMMAND /bin/sh -c "\"$<TARGET_FILE:astercli>\" --connect invalid PING 2>&1; test $? -ne 0"
    )

    set_tests_properties(astercli_rejects_invalid_connect_endpoint
            PROPERTIES
                PASS_REGULAR_EXPRESSION "Invalid connect address"
    )
endif ()
