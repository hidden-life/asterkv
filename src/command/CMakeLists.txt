file(
        GLOB_RECURSE
        ASTERKV_COMMAND_SOURCES
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

file(
        GLOB_RECURSE
        ASTERKV_COMMAND_PUBLIC_HEADERS
        CONFIGURE_DEPENDS
        "${PROJECT_SOURCE_DIR}/include/asterkv/command/*.h"
)

add_library(asterkv_command STATIC)

add_library(AsterKV::Command ALIAS asterkv_command)

target_sources(asterkv_command
        PRIVATE
            ${ASTERKV_COMMAND_SOURCES}

        PUBLIC
            FILE_SET publicHeaders
            TYPE HEADERS
            BASE_DIRS
                "${PROJECT_SOURCE_DIR}/include"
            FILES
                ${ASTERKV_COMMAND_PUBLIC_HEADERS}
)

target_include_directories(asterkv_command
        PUBLIC
            "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
            "$<INSTALL_INTERFACE:include>"
)

target_link_libraries(asterkv_command PUBLIC
        AsterKV::Core
)

asterkvApplyCompilerOptions(asterkv_command)
