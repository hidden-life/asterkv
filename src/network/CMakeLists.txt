file(
        GLOB_RECURSE
        ASTERKV_NETWORK_SOURCES
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

file(
        GLOB_RECURSE
        ASTERKV_NETWORK_PUBLIC_HEADERS
        CONFIGURE_DEPENDS
        "${PROJECT_SOURCE_DIR}/include/asterkv/network/*.h"
)

add_library(asterkv_network STATIC)

add_library(AsterKV::Network ALIAS asterkv_network)

target_sources(asterkv_network
        PRIVATE
            ${ASTERKV_NETWORK_SOURCES}
        PUBLIC
            FILE_SET publicHeaders
            TYPE HEADERS
            BASE_DIRS
                "${PROJECT_SOURCE_DIR}/include"
            FILES
                ${ASTERKV_NETWORK_PUBLIC_HEADERS}
)

target_include_directories(asterkv_network
        PUBLIC
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:include>"
)

target_link_libraries(asterkv_network
        PUBLIC
            AsterKV::Core
            AsterKV::Pipeline
)

asterkvApplyCompilerOptions(asterkv_network)
