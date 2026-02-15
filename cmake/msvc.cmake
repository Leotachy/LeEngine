
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/MP>)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

function (LeEngine_target_settings_toolchain target)

    target_compile_definitions(${target} PUBLIC $<$<COMPILE_LANGUAGE:CXX>:_CRT_SECURE_NO_WARNINGS>)

    # Set warning level 3 and enable warnings as errors
    target_compile_options(${target} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:/W3>)
    target_compile_options(${target} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:/WX>)

    # Disable unreachable warning (libfmt)
    target_compile_options(${target} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:/wd4702>) # unreachable

    # Treat code included using angle brackets as external, and set warning level 3 for external code
    target_compile_options(${target} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:/external:anglebrackets>)
    target_compile_options(${target} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:/external:W3>)

    # Source code is UTF-8
    target_compile_options(${target} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:/utf-8>)

endfunction()