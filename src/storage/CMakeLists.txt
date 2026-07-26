file(
        GLOB_RECURSE
        ASTERKV_STORAGE_SOURCES
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

file(
        GLOB_RECURSE
        ASTERKV_STORAGE_PUBLIC_HEADERS
        CONFIGURE_DEPENDS
        "${PROJECT_SOURCE_DIR}/include/asterkv/storage/*.h"
)

add_library(asterkv_storage STATIC)

add_library(AsterKV::Storage ALIAS asterkv_storage)

target_sources(asterkv_storage
        PRIVATE
            ${ASTERKV_STORAGE_SOURCES}
        PUBLIC
            FILE_SET publicHeaders
            TYPE HEADERS
            BASE_DIRS
                "${PROJECT_SOURCE_DIR}/include"
            FILES
                ${ASTERKV_STORAGE_PUBLIC_HEADERS}
)

target_include_directories(asterkv_storage
        PUBLIC
            "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
            "$<INSTALL_INTERFACE:include>"
)

target_link_libraries(asterkv_storage
        PUBLIC
        AsterKV::Core
)

asterkvApplyCompilerOptions(asterkv_storage)
