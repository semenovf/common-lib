////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Vladislav Trifochkin
//
// This file is part of [common-lib](https://github.com/semenovf/common-lib) library.
//
// Changelog:
//      2019.12.23 Initial version.
//      2021.11.23 Refactored excluding use of external cmake script.
////////////////////////////////////////////////////////////////////////////////
#pragma once

// NOTE. Visual Studio supports the new C++17 <filesystem> standard since
// Visual Studio 2017 version 15.7 (_MSC_VER >= 1914, _MSC_FULL_VER >= 191426428).

#if (defined(__cplusplus) && __cplusplus >= 201703L               \
        && defined(__has_include) && __has_include(<filesystem>)) \
        || (defined(_MSC_VER) && _MSC_VER >= 1914)
#   define PFS_STD_FILESYSTEM_ENABLED 1
#   include <filesystem>
namespace pfs {
    namespace filesystem = std::filesystem;
} // namespace pfs
#else
#   if defined(_MSC_VER)
#       define GHC_WIN_WSTRING_STRING_TYPE
#   endif
#   include "3rdparty/ghc/filesystem.hpp"
namespace pfs {
    namespace filesystem = ghc::filesystem;
} // namespace pfs
#endif
