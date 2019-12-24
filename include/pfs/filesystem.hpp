////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 Vladislav Trifochkin
//
// This file is part of [pfs-common](https://github.com/semenovf/pfs-common) library.
//
// Changelog:
//      2019.12.23 Initial version
////////////////////////////////////////////////////////////////////////////////
#ifndef HAVE_STD_FILESYSTEM

#if defined(__cplusplus) && __cplusplus >= 201703L && defined(__has_include) && __has_include(<filesystem>)
#   define HAVE_STD_FILESYSTEM 1
#endif

#endif // HAVE_STD_FILESYSTEM

#if HAVE_STD_FILESYSTEM
#   include <filesystem>
namespace pfs {
    namespace filesystem = std::filesystem;
} // namespace pfs
#else
#   include "3rdparty/ghc/filesystem.hpp"
namespace pfs {
    namespace filesystem = ghc::filesystem;
} // namespace pfs
#endif
