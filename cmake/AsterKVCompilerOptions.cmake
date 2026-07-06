function(asterkv_apply_compiler_options target_name)
    target_compile_features(${target_name} PUBLIC cxx_std_23)

    set_target_properties(${taget_name}
            PROPERTIES
            CXX_STANDARD 23
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
    )

    if (MSVC)
        target_compile_options(${target_name} PRIVATE
                /W4
                /permissive-
        )
    else ()
        target_compile_options(${target_name} PRIVATE
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
