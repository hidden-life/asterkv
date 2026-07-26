file(
        GLOB_RECURSE
        ASTERKV_EXECUTION_SOURCES
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

file(
        GLOB_RECURSE
        ASTERKV_EXECUTION_PUBLIC_HEADERS
        CONFIGURE_DEPENDS
        "${PROJECT_SOURCE_DIR}/include/asterkv/execution/*.h"
)

add_library(asterkv_execution STATIC)

add_library(AsterKV::Execution ALIAS asterkv_execution)

target_sources(asterkv_execution
        PRIVATE
            ${ASTERKV_EXECUTION_SOURCES}
        PUBLIC
            FILE_SET publicHeaders
            TYPE HEADERS
            BASE_DIRS
                "${PROJECT_SOURCE_DIR}/include"
            FILES
                ${ASTERKV_EXECUTION_PUBLIC_HEADERS}
)

target_include_directories(asterkv_execution
        PUBLIC
            "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
            "$<INSTALL_INTERFACE:include>"
)

target_link_libraries(asterkv_execution
        PUBLIC
            AsterKV::Command
            AsterKV::Storage
)

asterkvApplyCompilerOptions(asterkv_execution)
