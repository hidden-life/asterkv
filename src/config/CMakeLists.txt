file(
        GLOB_RECURSE
        ASTERKV_CONFIG_SOURCES
        CONFIGURE_DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

file(
        GLOB_RECURSE
        ASTERKV_CONFIG_PUBLIC_HEADERS
        CONFIGURE_DEPENDS
            "${PROJECT_SOURCE_DIR}/include/asterkv/config/*.h"
)

add_library(asterkv_config STATIC)

add_library(AsterKV::Config ALIAS asterkv_config)

target_sources(asterkv_config
        PRIVATE
            ${ASTERKV_CONFIG_SOURCES}

        PUBLIC
            FILE_SET publicHeaders
            TYPE HEADERS
            BASE_DIRS
                "${PROJECT_SOURCE_DIR}/include"
            FILES
                ${ASTERKV_CONFIG_PUBLIC_HEADERS}
)

target_include_directories(asterkv_config
        PUBLIC
            "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
            "$<INSTALL_INTERFACE:include>"
)

target_link_libraries(asterkv_config
        PUBLIC
            AsterKV::Core
            AsterKV::Network
            AsterKV::Server
)
