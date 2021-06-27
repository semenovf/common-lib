////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 Vladislav Trifochkin
//
// This file is part of [common-lib](https://github.com/semenovf/common-lib) library.
//
// Changelog:
//      2019.12.23 Initial version
////////////////////////////////////////////////////////////////////////////////
#pragma once

#if !defined(PFS_NO_STD_FILESYSTEM)
#   include <filesystem>
#else
#   if defined(_WIN32) || defined(_WIN64)
#       define GHC_WIN_WSTRING_STRING_TYPE
#   endif
#   include "3rdparty/ghc/filesystem.hpp"
namespace pfs {
    namespace filesystem = ghc::filesystem;
} // namespace pfs
#endif
