file(
        GLOB_RECURSE
        ASTERKV_CORE_SOURCES
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

file(
        GLOB_RECURSE
        ASTERKV_CORE_PUBLIC_HEADERS
        CONFIGURE_DEPENDS
        "${PROJECT_SOURCE_DIR}/include/asterkv/core/*.h"
)

add_library(asterkv_core STATIC)
add_library(AsterKV::Core ALIAS asterkv_core)

target_sources(asterkv_core
        PRIVATE
            ${ASTERKV_CORE_SOURCES}

        PUBLIC
            FILE_SET publicHeaders
            TYPE HEADERS
            BASE_DIRS
                "${PROJECT_SOURCE_DIR}/include"
            FILES
                ${ASTERKV_CORE_PUBLIC_HEADERS}
)

target_include_directories(asterkv_core
        PUBLIC
            "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
            "$<INSTALL_INTERFACE:include>"
)

asterkvApplyCompilerOptions(asterkv_core)
