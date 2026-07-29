file(GLOB_RECURSE ASTERKV_SERVER_SOURCES
        CONFIGURE_DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

file(GLOB_RECURSE ASTERKV_SERVER_PUBLIC_HEADERS
        CONFIGURE_DEPENDS
            "${PROJECT_SOURCE_DIR}/include/asterkv/server/*.h"
)

add_library(asterkv_server STATIC)

add_library(AsterKV::Server ALIAS asterkv_server)

target_sources(asterkv_server
        PRIVATE
            ${ASTERKV_SERVER_SOURCES}

        PUBLIC
            FILE_SET publicHeaders
            TYPE HEADERS
            BASE_DIRS
                "${PROJECT_SOURCE_DIR}/include"
            FILES
                ${ASTERKV_SERVER_PUBLIC_HEADERS}
)

target_include_directories(asterkv_server
        PUBLIC
            "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
            "$<INSTALL_INTERFACE:include>"
)

target_link_libraries(asterkv_server
        PUBLIC
            AsterKV::Core
            AsterKV::Network
            AsterKV::Pipeline
            AsterKV::Storage
            AsterKV::Logging
)

asterkvApplyCompilerOptions(asterkv_server)
