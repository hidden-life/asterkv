add_executable(asterd
        main.cpp
)


target_link_libraries(asterd
        PRIVATE
        AsterKV::Core
)

asterkvApplyCompilerOptions(asterd)

add_test(
        NAME asterd_prints_version
        COMMAND asterd --version
)
