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
            COMMAND /bin/sh -c "\"$<TARGET_FILE:astercli>\" --local \"NOPE key\" 2>&1; test $? -ne 0"
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

    add_test(
            NAME astercli_local_success_exit_code
            COMMAND /bin/sh -c "stderr_file=\"$(mktemp)\"; output=\"$($<TARGET_FILE:astercli> --local PING 2>\"$stderr_file\")\"; status=$?; test \"$status\" -eq 0 && printf '%s' \"$output\" | grep -F -- PONG >/dev/null && test ! -s \"$stderr_file\"; result=$?; rm -f \"$stderr_file\"; exit \"$result\""
    )

    add_test(
            NAME astercli_local_protocol_error_exit_code
            COMMAND /bin/sh -c "stderr_file=\"$(mktemp)\"; stdout=\"$($<TARGET_FILE:astercli> --local \"NOPE key\" 2>\"$stderr_file\")\"; status=$?; test \"$status\" -ne 0 && test -z \"$stdout\" && grep -F -- \"error: invalid_argument unknown command\" \"$stderr_file\" >/dev/null; result=$?; rm -f \"$stderr_file\"; exit \"$result\""
    )
endif ()
