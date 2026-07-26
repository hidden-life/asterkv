find_program(ASTERKV_NC_EXECUTABLE nc)
find_program(ASTERKV_TIMEOUT_EXECUTABLE timeout)

if (ASTERKV_NC_EXECUTABLE AND ASTERKV_TIMEOUT_EXECUTABLE)
    add_test(
            NAME asterd_tcp_smoke_test
            COMMAND
                /bin/sh
                "${CMAKE_CURRENT_SOURCE_DIR}/tcp_server_smoke_test.sh"
                "$<TARGET_FILE:asterd>"
                "${ASTERKV_NC_EXECUTABLE}"
                "${ASTERKV_TIMEOUT_EXECUTABLE}"
                "127.0.0.1"
                "17721"
    )

    set_tests_properties(asterd_tcp_smoke_test
            PROPERTIES
                LABELS "integration;network"
                TIMEOUT 30
    )
elseif (ASTERKV_REQUIRE_TEST_DEPENDENCIES)
    message(FATAL_ERROR "TCP integration tests require both 'nc' and 'timeout'.")
else ()
    message(STATUS "TCP integration smoke test disabled because 'nc' or 'timeout' was not found.")
endif ()
