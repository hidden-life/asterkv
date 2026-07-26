add_executable(astercli
        main.cpp
)

target_link_libraries(astercli
        PRIVATE
        AsterKV::Core
        AsterKV::Pipeline
        AsterKV::Storage
)

asterkvApplyCompilerOptions(astercli)

if (BUILD_TESTING)
    add_test(
            NAME astercli_prints_version
            COMMAND astercli --version
    )

    add_test(
            NAME astercli_local_ping
            COMMAND astercli local PING
    )

    set_tests_properties(
            astercli_local_ping
            PROPERTIES
                PASS_REGULAR_EXPRESSION "\\+PONG"
    )

    add_test(
            NAME astercli_local_set
            COMMAND astercli local "SET username alex"
    )

    set_tests_properties(
            astercli_local_set
            PROPERTIES
                PASS_REGULAR_EXPRESSION "\\+OK"
    )

    add_test(
            NAME astercli_local_unknown_command
            COMMAND astercli local "NOPE key"
    )

    set_tests_properties(
            astercli_local_unknown_command
            PROPERTIES
                PASS_REGULAR_EXPRESSION "-ERR invalid_argument unknown command"
    )
endif ()
