add_executable(asterd
        main.cpp
)


target_link_libraries(asterd
        PRIVATE
        AsterKV::Core
)

asterkv_apply_compiler_options(asterd)

add_test(
        NAME asterd_prints_version
        COMMAND asterd --version
)
