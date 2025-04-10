[![Standard](https://img.shields.io/badge/C%2B%2B-11%2F14%2F17-blue)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![Build status](https://ci.appveyor.com/api/projects/status/bj7iu6bi1hf3r4w3?svg=true)](https://ci.appveyor.com/project/semenovf/common-lib)

# common-lib (Common library for Portable Foundation Structures)

## Quick Reference

### pfs::any

Purpose: a type-safe container for single values of any copy constructible type.
See also: std::any (C++17)
Details: it is a typedef for `std::any` (if the compiler meets the requirements of at least the C++17 standard) 
or it is typedef for `stx::any` () otherwise
Usage example:
```sh
```

### pfs::argvapi

Purpose: command line interface simple API.

## I18N support

Call cmake with `PFS__ENABLE_NLS` to enable NLS support:

```sh
$ cmake ... -DPFS__ENABLE_NLS=ON ...
```

```cmake
...

if (TARGET <TARGET>)
    # Target `libintl` available if `PFS__ENABLE_NLS` is ON 
    # and `PFS__USE_IMPORTED_GETTEXT_LIB` is ON (on Windows platform)
    if (TARGET libintl)
        portable_target(LINK <TARGET> PRIVATE libintl)
        
        add_custom_command(TARGET <TARGET>
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy 
                "$<TARGET_PROPERTY:libintl,IMPORTED_LOCATION>"
                "$<TARGET_FILE_DIR:<TARGET>>"
            COMMAND ${CMAKE_COMMAND} -E copy 
                "$<TARGET_PROPERTY:libintl,ICONV_LIB>"
                "$<TARGET_FILE_DIR:<TARGET>>"
            VERBATIM)
    endif()
endif()
...
```

# Install ICU

## Windows

Required ICU version 71.1.
Build from sources required `python 3`.

Note. The path to sources must not contain spaces. There was a problem building ICU library.

### Using prebuilt binaries (not implemented yet)

1. Download them from `git` repository:
* [Win32-MSVC2019](https://github.com/unicode-org/icu/releases/download/release-71-1/icu4c-71_1-Win32-MSVC2019.zip)
* [Win64-MSVC2019](https://github.com/unicode-org/icu/releases/download/release-71-1/icu4c-71_1-Win64-MSVC2019.zip)
