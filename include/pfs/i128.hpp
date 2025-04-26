////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2021.11.08 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <cstdint>

#ifndef PFS__HAS_INT128
#   if defined(__SIZEOF_INT128__) && !defined(__NVCC__) && !(__clang__ && _MSC_VER)
#       define PFS__HAS_INT128 1
#   endif
#endif

// See https://stackoverflow.com/questions/16088282/is-there-a-128-bit-integer-in-gcc
#if PFS__HAS_INT128
    using int128_type = __int128_t;
    using uint128_type = __uint128_t;
#endif

namespace pfs {

#if PFS__HAS_INT128
inline constexpr __uint128_t construct_uint128 (std::uint64_t hi, std::uint64_t lo) noexcept
{
    return (static_cast<__uint128_t>(hi) << 64) | lo;
}
#endif

} // namespace pfs
