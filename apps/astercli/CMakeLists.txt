add_executable(astercli
        main.cpp
)

target_link_libraries(astercli
        PRIVATE
        AsterKV::Core
)

asterkv_apply_compiler_options(astercli)

add_test(
        NAME astercli_prints_version
        COMMAND astercli --version
)
