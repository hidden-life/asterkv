function(asterkvApplyCompilerOptions targetName)
    target_compile_features(${targetName} PUBLIC cxx_std_23)

    set_target_properties(${targetName}
            PROPERTIES
            CXX_STANDARD 23
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
    )

    if (MSVC)
        target_compile_options(${targetName} PRIVATE
                /W4
                /permissive-
        )
    else ()
        target_compile_options(${targetName} PRIVATE
                -Wall
                -Wextra
                -Wpedantic
                -Wconversion
                -Wsign-conversion
                -Wshadow
                -Wold-style-cast
                -Wnon-virtual-dtor
                -Woverloaded-virtual
        )
    endif ()
endfunction()
