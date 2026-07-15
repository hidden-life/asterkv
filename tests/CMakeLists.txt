add_subdirectory(headers)

if (ASTERKV_BUILD_UNIT_TESTS)
    find_package(GTest CONFIG QUIET)

    if (GTest_FOUND)
        add_subdirectory(core)
    elseif (ASTERKV_REQUIRE_TEST_DEPENDENCIES)
        message(FATAL_ERROR "GoogleTest was not found. Install GoogleTest or configure with -DASTERKV_BUILD_UNIT_TESTS=OFF.")
    else ()
        message(STATUS "GoogleTest was not found; GoogleTest-based unit tests are disabled.")
    endif ()
endif ()
