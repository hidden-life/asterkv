file(
        GLOB_RECURSE
        ASTERKV_CLIENT_SOURCES
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

file(
        GLOB_RECURSE
        ASTERKV_CLIENT_PUBLIC_HEADERS
        CONFIGURE_DEPENDS
        "${PROJECT_SOURCE_DIR}/include/asterkv/client/*.h"
)

add_library(asterkv_client STATIC)

add_library(AsterKV::Client ALIAS asterkv_client)

target_sources(asterkv_client
        PRIVATE
            ${ASTERKV_CLIENT_SOURCES}

        PUBLIC
            FILE_SET publicHeaders
            TYPE HEADERS
            BASE_DIRS
                "${PROJECT_SOURCE_DIR}/include"
            FILES
                ${ASTERKV_CLIENT_PUBLIC_HEADERS}
)

target_include_directories(asterkv_client
        PUBLIC
            "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
            "$<INSTALL_INTERFACE:include>"
)

target_link_libraries(asterkv_client
        PUBLIC
            AsterKV::Core
)
