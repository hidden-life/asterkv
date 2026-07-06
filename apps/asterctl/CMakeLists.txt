add_executable(asterctl
        main.cpp
)

target_link_libraries(asterctl
        PRIVATE
        AsterKV::Core
)

asterkv_apply_compiler_options(asterctl)

add_test(
        NAME asterctl_prints_version
        COMMAND asterctl --version
)
