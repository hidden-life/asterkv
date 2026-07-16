cmake_minimum_required(VERSION 3.25)

project(
        AsterKV
        VERSION 0.1.0
        DESCRIPTION "A distributed high-performance key-value database written in modern C++"
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

option(ASTERKV_BUILD_UNIT_TESTS "Build AsterKV unit tests" ${BUILD_TESTING})
option(ASTERKV_REQUIRE_TEST_DEPENDENCIES "Fail configuration when test dependencies are missing" OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(AsterKVCompilerOptions)

add_subdirectory(src/core)
add_subdirectory(src/command)

add_subdirectory(apps/asterd)
add_subdirectory(apps/astercli)
add_subdirectory(apps/asterctl)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif ()
