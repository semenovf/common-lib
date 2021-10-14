////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of [common-lib](https://github.com/semenovf/common-lib) library.
//
// Changelog:
//      2021.10.14 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "pfs/bits/compiler.h"
#include <cstdint>

#if PFS_COMPILER_MSC
#   include <stdlib.h>
#endif

// May be more accurate method is needed for PFS_BYTESWAP_CONSTEXPR definition
#ifndef PFS_BYTESWAP_CONSTEXPR
#   if __cplusplus >= 201402L
#       define PFS_BYTESWAP_CONSTEXPR constexpr
#   else
#       define PFS_BYTESWAP_CONSTEXPR
#   endif
#endif // ! PFS_BYTESWAP_CONSTEXPR

#if !defined(PFS_HAS_INT128) && defined(__SIZEOF_INT128__)
#   define PFS_HAS_INT128 1
#endif

namespace pfs {

#if defined(PFS_HAS_INT128)
inline constexpr __uint128_t construct_uint128 (std::uint64_t hi
    , std::uint64_t low) noexcept
{
    return (static_cast<__uint128_t>(hi) << 64) | low;
}
#endif

template <typename T>
PFS_BYTESWAP_CONSTEXPR T byteswap (T n) noexcept;

template <>
inline PFS_BYTESWAP_CONSTEXPR
std::uint8_t byteswap<std::uint8_t> (std::uint8_t x) noexcept
{
    return x;
}

#if PFS_COMPILER_GNUC

#   define PFS_BYTESWAP_USE_INTRINSICS 1

template <>
inline PFS_BYTESWAP_CONSTEXPR
std::uint16_t byteswap<std::uint16_t> (std::uint16_t x) noexcept
{
    return __builtin_bswap16(x);
}

template <>
inline PFS_BYTESWAP_CONSTEXPR
std::uint32_t byteswap<std::uint32_t> (std::uint32_t x) noexcept
{
    return __builtin_bswap32(x);
}

template <>
inline PFS_BYTESWAP_CONSTEXPR
std::uint64_t byteswap<std::uint64_t> (std::uint64_t x) noexcept
{
    return __builtin_bswap64(x);
}

#elif PFS_COMPILER_MSC
#   define PFS_BYTESWAP_USE_INTRINSICS 1

// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference
//      /byteswap-uint64-byteswap-ulong-byteswap-ushort?view=msvc-160

template <>
inline PFS_BYTESWAP_CONSTEXPR
std::uint16_t byteswap<std::uint16_t> (std::uint16_t x) noexcept
{
    return _byteswap_ushort(x);
}

template <>
inline PFS_BYTESWAP_CONSTEXPR
std::uint32_t byteswap<std::uint32_t> (std::uint32_t x) noexcept
{
    return _byteswap_ulong(x);
}

template <>
inline PFS_BYTESWAP_CONSTEXPR
std::uint64_t byteswap<std::uint64_t> (std::uint64_t x) noexcept
{
    return _byteswap_uint64(x);
}

#endif

#ifndef PFS_BYTESWAP_USE_INTRINSICS
template <>
inline PFS_BYTESWAP_CONSTEXPR
std::uint16_t byteswap<std::uint16_t> (std::uint16_t x) noexcept
{
    return (x >> 8) | (x << 8);;
}

template <>
inline PFS_BYTESWAP_CONSTEXPR
std::uint32_t byteswap<std::uint32_t> (std::uint32_t x) noexcept
{
    std::uint32_t y = ((x << 8) & 0xFF00FF00) | ((x >> 8) & 0xFF00FF);
    return (y << 16) | (y >> 16);
}

template <>
inline PFS_BYTESWAP_CONSTEXPR
std::uint64_t byteswap<std::uint64_t> (std::uint64_t x) noexcept
{
    x = ((x & 0x00000000FFFFFFFFull) << 32) | ((x & 0xFFFFFFFF00000000ull) >> 32);
    x = ((x & 0x0000FFFF0000FFFFull) << 16) | ((x & 0xFFFF0000FFFF0000ull) >> 16);
    x = ((x & 0x00FF00FF00FF00FFull) << 8)  | ((x & 0xFF00FF00FF00FF00ull) >> 8);
    return x;
}
#endif // ! PFS_BYTESWAP_USE_INTRINSICS

#if defined(PFS_HAS_INT128)
template <>
inline PFS_BYTESWAP_CONSTEXPR
__uint128_t byteswap<__uint128_t> (__uint128_t x) noexcept
{
    return construct_uint128(byteswap(static_cast<std::uint64_t>(x >> 64))
        , byteswap(static_cast<std::uint64_t>(x)));
}
#endif

} // namespace pfs
