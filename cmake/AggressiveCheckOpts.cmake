################################################################################
# Copyright (c) 2025 Vladislav Trifochkin
#
# Changelog:
#      2025.08.18 Initial version.
#      2025.11.17 Added `IS_SANITIZE_THREAD` parameters.
################################################################################
function (aggressive_check_opts TARGET IS_SANITIZE_THREAD)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${TARGET} PRIVATE
            "-D_GLIBCXX_DEBUG=1"
            "-D_GLIBCXX_DEBUG_PEDANTIC=1"
            "-D_FORTIFY_SOURCE=2"
            "-pedantic"
            "-O2"
            "-Wall"
            "-Wextra"
            "-Wshadow"
            "-Wformat=2"
            "-Wfloat-equal"
            # "-Wconversion" # <-- Annoying message, may be need separate option for this
            "-Wlogical-op"
            "-Wshift-overflow=2"
            "-Wduplicated-cond"
            "-Wcast-qual"
            "-Wcast-align")

        if (${IS_SANITIZE_THREAD})
            target_compile_options(${TARGET} PRIVATE
                "-fno-sanitize-recover"
                "-fstack-protector"
                # gcc: error: -fsanitize=address and -fsanitize=kernel-address are incompatible with -fsanitize=thread
                "-fsanitize=thread"
            )

            target_link_libraries(${TARGET} PRIVATE
                "-fsanitize=thread"
                "-ltsan"  # <-- need for -fsanitize=thread
            )
        else()
            target_compile_options(${TARGET} PRIVATE
                "-fsanitize=address"   # <-- The option cannot be combined with -fsanitize=thread and/or -fcheck-pointer-bounds.
                "-fsanitize=undefined"
                "-fsanitize=leak"      # <-- The option cannot be combined with -fsanitize=thread
                "-fno-sanitize-recover"
                "-fstack-protector"

                # gcc: error: -fsanitize=address and -fsanitize=kernel-address are incompatible with -fsanitize=thread
                # "-fsanitize=thread"
            )

            target_link_libraries(${TARGET} PRIVATE
                "-fsanitize=address"
                "-fsanitize=undefined"
                "-fsanitize=leak"

                "-lasan"  # <-- need for -fsanitize=address
                "-lubsan" # <-- need for -fsanitize=undefined
            )
        endif()
    endif()
endfunction(aggressive_check_opts)
