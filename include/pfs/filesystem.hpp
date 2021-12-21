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

#if defined(PFS_COMPILER_MSVC)
#   include "windows.hpp"
#endif

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
#   include "3rdparty/ghc/filesystem.hpp"
namespace pfs {
namespace filesystem {
    using namespace ghc::filesystem;
}} // namespace pfs::filesystem
#endif

#ifndef PFS_PLATFORM_LITERAL
#   if defined(PFS_COMPILER_MSVC)
#       if !(defined(_UNICODE) || defined(UNICODE))
#           error "Expected _UNICODE(UNICODE) is enabled"
#       endif
#       define PFS_PLATFORM_LITERAL(x) L##x
#   else
#       define PFS_PLATFORM_LITERAL(x) x
#   endif
#endif

#ifndef PFS_UTF8_ENCODE_PATH
#   if defined(PFS_COMPILER_MSVC)
#       if !(defined(_UNICODE) || defined(UNICODE))
#           error "Expected _UNICODE(UNICODE) is enabled"
#       endif
#       define PFS_UTF8_ENCODE_PATH(x) utf8_encode(x)
#   else
#       define PFS_UTF8_ENCODE_PATH(x) x
#   endif
#endif

#ifndef PFS_UTF8_DECODE_PATH
#   if defined(PFS_COMPILER_MSVC)
#       if !(defined(_UNICODE) || defined(UNICODE))
#           error "Expected _UNICODE(UNICODE) is enabled"
#       endif
#       define PFS_UTF8_DECODE_PATH(x) utf8_decode(x)
#   else
#       define PFS_UTF8_DECODE_PATH(x) x
#   endif
#endif
