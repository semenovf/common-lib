////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2019.12.23 Initial version.
//      2021.11.20 Refactored excluding use of external cmake script.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "bits/compiler.h"
#include "c++support.hpp"
#include "fmt.hpp"

#if defined(PFS__COMPILER_MSVC)
#   include "windows.hpp"
#endif

// NOTE. Visual Studio supports the new C++17 <filesystem> standard since
// Visual Studio 2017 version 15.7 (_MSC_VER >= 1914, _MSC_FULL_VER >= 191426428).

#if defined(__cplusplus) && __cplusplus >= 201703L
#   if defined(PFS__COMPILER_MSVC) && PFS__COMPILER_MSVC_VERSION >= 1914
#       define PFS_HAVE_STD_FILESYSTEM 1
#   elif defined(PFS__COMPILER_GCC) && PFS__COMPILER_GCC_VERSION >= 80000
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

#ifndef PFS__LITERAL_PATH
#   if defined(PFS__COMPILER_MSVC)
#       if !(defined(_UNICODE) || defined(UNICODE))
#           error "Expected _UNICODE/UNICODE is enabled"
#       endif
#       define PFS__LITERAL_PATH(x) L##x
#   else
#       define PFS__LITERAL_PATH(x) x
#   endif
#endif

namespace pfs {

#if PFS_HAVE_STD_FILESYSTEM
using _path_t = std::filesystem::path;
#else
using _path_t = pfs::filesystem::path;
#endif

#if defined(PFS__COMPILER_MSVC)
#   if !(defined(_UNICODE) || defined(UNICODE))
#       error "Expected _UNICODE/UNICODE is enabled"
#   endif
inline std::string utf8_encode_path (_path_t const & p)
{
    return pfs::windows::utf8_encode(p.c_str());
}

inline _path_t utf8_decode_path (std::string const & s)
{
    return _path_t{pfs::windows::utf8_decode(s.c_str(), static_cast<int>(s.size()))};
}

#else
inline std::string utf8_encode_path (_path_t const & p)
{
    return p.native();
}

inline _path_t utf8_decode_path (std::string const & s)
{
    return _path_t{s};
}

#endif

} // namespace filesystem


namespace pfs {
namespace filesystem {

/**
 * Use pfs::utf8_encode_path instead
 */
PFS__DEPRECATED inline std::string utf8_encode (path const & p)
{
    return pfs::utf8_encode_path(p);
}

/**
 * Use pfs::utf8_decode_path instead
 */
PFS__DEPRECATED inline path utf8_decode (std::string const & s)
{
    return utf8_decode_path(s);
}

}} // namespace pfs::filesystem

namespace fmt {

template <>
struct formatter<pfs::filesystem::path>
{
    template <typename ParseContext>
    /*constexpr */auto parse (ParseContext & ctx) -> decltype(ctx.begin())
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format (pfs::filesystem::path const & path, FormatContext & ctx) const -> decltype(ctx.out())
    {
        return format_to(ctx.out(), "{}", pfs::utf8_encode_path(path));
    }
};

} // namespace fmt

#ifndef PFS_HAVE_STD_FILESYSTEM
namespace std {

template<>
struct hash<pfs::filesystem::path>
{
    std::size_t operator () (pfs::filesystem::path const & path) const noexcept
    {
        return pfs::filesystem::hash_value(path);
    }
};

} // namespace std
#endif // !PFS_HAVE_STD_FILESYSTEM
