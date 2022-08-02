[![Standard](https://img.shields.io/badge/C%2B%2B-11%2F14%2F17-blue)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![Build Status](https://travis-ci.com/semenovf/common-lib.svg?branch=master)](https://travis-ci.com/semenovf/common-lib)
[![Build status](https://ci.appveyor.com/api/projects/status/bj7iu6bi1hf3r4w3?svg=true)](https://ci.appveyor.com/project/semenovf/common-lib)

# common-lib (Common library for Portable Foundation Structures)

namespace pfs {<br/>
    1. `dynamic_library` class<br/>
    1. `function_queue` container class<br/>
    2. `filesystem` classes<br/>
    3. `fmt`<br/>
    4. `iterator` classes<br/>
    5. `numeric` functions<br/>
    6. `rational` class<br/>
    7. `variant` class and functions<br/>
    8. `static_unique_pointer_cast` function<br/>
    9. `unicode`<br/>
} // namespace pfs<br/>

# I18N support

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