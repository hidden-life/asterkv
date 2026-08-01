file(
        GLOB_RECURSE
        ASTERKV_WAL_SOURCES
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

file(
        GLOB_RECURSE
        ASTERKV_WAL_PUBLIC_HEADERS
        CONFIGURE_DEPENDS
        "${PROJECT_SOURCE_DIR}/include/asterkv/wal/*.h"
)

add_library(asterkv_wal STATIC)

add_library(AsterKV::Wal ALIAS asterkv_wal)

target_sources(asterkv_wal
        PRIVATE
            ${ASTERKV_WAL_SOURCES}

        PUBLIC
            FILE_SET publicHeaders
            TYPE HEADERS
            BASE_DIRS
                "${PROJECT_SOURCE_DIR}/include"
            FILES
                ${ASTERKV_WAL_PUBLIC_HEADERS}
)

target_include_directories(asterkv_wal
        PUBLIC
            "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
            "$<INSTALL_INTERFACE:include>"
)

target_link_libraries(asterkv_wal
        PUBLIC
            AsterKV::Core
)

asterkvApplyCompilerOptions(asterkv_wal)
