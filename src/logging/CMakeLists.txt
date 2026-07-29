file(
        GLOB_RECURSE
        ASTERKV_LOGGING_SOURCES
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

file(
        GLOB_RECURSE
        ASTERKV_LOGGING_PUBLIC_HEADERS
        CONFIGURE_DEPENDS
        "${PROJECT_SOURCE_DIR}/include/asterkv/logging/*.h"
)

find_package(spdlog CONFIG REQUIRED)

add_library(asterkv_logging STATIC)

add_library(AsterKV::Logging ALIAS asterkv_logging)

target_sources(asterkv_logging
        PRIVATE
            ${ASTERKV_LOGGING_SOURCES}

        PUBLIC
            FILE_SET publicHeaders
            TYPE HEADERS
            BASE_DIRS
                "${PROJECT_SOURCE_DIR}/include"
            FILES
                ${ASTERKV_LOGGING_PUBLIC_HEADERS}
)

target_include_directories(asterkv_logging
        PUBLIC
            "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
            "$<INSTALL_INTERFACE:include>"
)

target_link_libraries(asterkv_logging
        PUBLIC
            AsterKV::Core

            spdlog::spdlog
)

asterkvApplyCompilerOptions(asterkv_logging)
