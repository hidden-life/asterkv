file(
        GLOB_RECURSE
        ASTERKV_PROTOCOL_SOURCES
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

file(
        GLOB_RECURSE
        ASTERKV_PROTOCOL_PUBLIC_HEADERS
        CONFIGURE_DEPENDS
        "${PROJECT_SOURCE_DIR}/include/asterkv/protocol/*.h"
)

add_library(asterkv_protocol STATIC)

add_library(AsterKV::Protocol ALIAS asterkv_protocol)

target_sources(asterkv_protocol
        PRIVATE
            ${ASTERKV_PROTOCOL_SOURCES}

        PUBLIC
            FILE_SET publicHeaders
            TYPE HEADERS
            BASE_DIRS
                "${PROJECT_SOURCE_DIR}/include"
            FILES
                ${ASTERKV_PROTOCOL_PUBLIC_HEADERS}
)

target_include_directories(asterkv_protocol
        PUBLIC
            "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
            "$<INSTALL_INTERFACE:include>"
)

target_link_libraries(asterkv_protocol
        PUBLIC
            AsterKV::Command
)

asterkvApplyCompilerOptions(asterkv_protocol)
