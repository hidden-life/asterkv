add_library(asterkv_core STATIC
        version.cpp
)

add_library(AsterKV::Core ALIAS asterkv_core)

target_include_directories(asterkv_core
        PUBLIC
        "${PROJECT_SOURCE_DIR}/include"
)

asterkv_apply_compiler_options(asterkv_core)
