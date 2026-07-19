file(
        GLOB_RECURSE
        ASTERKV_PIPELINE_SOURCES
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

file(
        GLOB_RECURSE
        ASTERKV_PIPELINE_PUBLIC_HEADERS
        CONFIGURE_DEPENDS
        "${PROJECT_SOURCE_DIR}/include/asterkv/pipeline/*.h"
)

add_library(asterkv_pipeline STATIC)

add_library(AsterKV::Pipeline ALIAS asterkv_pipeline)

target_sources(asterkv_pipeline
        PRIVATE
        ${ASTERKV_PIPELINE_SOURCES}

        PUBLIC
        FILE_SET publicHeaders
        TYPE HEADERS
        BASE_DIRS
            "${PROJECT_SOURCE_DIR}/include"
        FILES
            ${ASTERKV_PIPELINE_PUBLIC_HEADERS}
)

target_include_directories(asterkv_pipeline
        PUBLIC
            "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
            "$<INSTALL_INTERFACE:include>"
)

target_link_libraries(asterkv_pipeline
        PUBLIC
            AsterKV::Protocol
            AsterKV::Execution
            AsterKV::Storage
)
