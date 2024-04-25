////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2021.10.14 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "pfs/bits/compiler.h"
#include "pfs/i128.hpp"
#include <cstdint>

#if PFS__COMPILER_MSVC
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

namespace pfs {

template <typename T>
PFS_BYTESWAP_CONSTEXPR T byteswap (T n) noexcept;

template <>
inline PFS_BYTESWAP_CONSTEXPR
char byteswap<char> (char x) noexcept
{
    return x;
}

template <>
inline PFS_BYTESWAP_CONSTEXPR
bool byteswap<bool> (bool x) noexcept
{
    return x;
}

template <>
inline PFS_BYTESWAP_CONSTEXPR
std::uint8_t byteswap<std::uint8_t> (std::uint8_t x) noexcept
{
    return x;
}

template <>
inline PFS_BYTESWAP_CONSTEXPR
std::int8_t byteswap<std::int8_t> (std::int8_t x) noexcept
{
    return x;
}

#if PFS__COMPILER_GCC

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

#elif PFS__COMPILER_MSVC
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

template <>
inline PFS_BYTESWAP_CONSTEXPR
std::int16_t byteswap<std::int16_t> (std::int16_t x) noexcept
{
    return static_cast<std::int16_t>(byteswap(static_cast<std::uint16_t>(x)));
}

template <>
inline PFS_BYTESWAP_CONSTEXPR
std::int32_t byteswap<std::int32_t> (std::int32_t x) noexcept
{
    return static_cast<std::int32_t>(byteswap(static_cast<std::uint32_t>(x)));
}

template <>
inline PFS_BYTESWAP_CONSTEXPR
std::int64_t byteswap<std::int64_t> (std::int64_t x) noexcept
{
    return static_cast<std::int64_t>(byteswap(static_cast<std::uint64_t>(x)));
}

#if defined(PFS_HAS_INT128)
template <>
inline PFS_BYTESWAP_CONSTEXPR
uint128_type byteswap<uint128_type> (uint128_type x) noexcept
{
    return construct_uint128(byteswap(static_cast<std::uint64_t>(x))
        , byteswap(static_cast<std::uint64_t>(x >> 64)));
}

template <>
inline PFS_BYTESWAP_CONSTEXPR
int128_type byteswap<int128_type> (int128_type x) noexcept
{
    return static_cast<int128_type>(byteswap(static_cast<uint128_type>(x)));
}
#endif // PFS_HAS_INT128

} // namespace pfs
