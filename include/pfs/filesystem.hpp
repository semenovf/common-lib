////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Vladislav Trifochkin
//
// This file is part of [common-lib](https://github.com/semenovf/common-lib) library.
//
// Changelog:
//      2019.12.23 Initial version.
//      2021.11.20 Refactored excluding use of external cmake script.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "bits/compiler.h"

// NOTE. Visual Studio supports the new C++17 <filesystem> standard since
// Visual Studio 2017 version 15.7 (_MSC_VER >= 1914, _MSC_FULL_VER >= 191426428).

#if defined(__cplusplus) && __cplusplus >= 201703L
#   if defined(PFS_COMPILER_MSVC) && PFS_COMPILER_MSVC_VERSION >= 1914
#       define PFS_HAVE_STD_FILESYSTEM 1
#   elif defined(PFS_COMPILER_GNUC) && PFS_COMPILER_GCC_VERSION >= 80000
#       define PFS_HAVE_STD_FILESYSTEM 1
#   endif
#endif
        
#if PFS_HAVE_STD_FILESYSTEM
#   include <filesystem>
namespace pfs {
namespace filesystem {
    using namespace std::filesystem;
}}
#else
#   if defined(_MSC_VER)
#       define GHC_WIN_WSTRING_STRING_TYPE
#   endif
#   include "3rdparty/ghc/filesystem.hpp"
namespace pfs {
namespace filesystem {
    using namespace ghc::filesystem;
}} // namespace pfs::filesystem
#endif
