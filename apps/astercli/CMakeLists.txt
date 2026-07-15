add_executable(astercli
        main.cpp
)

target_link_libraries(astercli
        PRIVATE
        AsterKV::Core
)

asterkvApplyCompilerOptions(astercli)

add_test(
        NAME astercli_prints_version
        COMMAND astercli --version
)
