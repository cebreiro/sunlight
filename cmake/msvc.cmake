if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

    add_compile_options(
        /WX
        /MP
    )

    if (CMAKE_CXX_FLAGS MATCHES "-fsanitize=address")
        add_link_options(/INCREMENTAL:NO)

    elseif (CMAKE_CONFIGURATION_TYPES STREQUAL RelWithDebInfo)
        add_compile_options(/wd4702)

    elseif (POLICY CMP0141)
        # Enable Hot Reload for MSVC compilers if supported.
        cmake_policy(SET CMP0141 NEW)
        set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")

    endif ()

endif()
