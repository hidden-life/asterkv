cmake_minimum_required(VERSION 3.25)
project(AsterKV
        VERSION 0.1.0
        DESCRIPTION "A distributed high-performance key-value database written in modern C++"
        LANGUAGES CXX
)

include(CTest)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(AsterKVCompilerOptions)

add_subdirectory(src/core)

add_subdirectory(apps/asterd)
add_subdirectory(apps/astercli)
add_subdirectory(apps/asterctl)
