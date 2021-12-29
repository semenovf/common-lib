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

// See https://stackoverflow.com/questions/16088282/is-there-a-128-bit-integer-in-gcc
#if !defined(PFS_HAS_INT128) && defined(__SIZEOF_INT128__)
#   define PFS_HAS_INT128 1
    using int128_type = __int128_t;
    using uint128_type = __uint128_t;
#endif

namespace pfs {

#if defined(PFS_HAS_INT128)
inline constexpr __uint128_t construct_uint128 (std::uint64_t hi
    , std::uint64_t lo) noexcept
{
    return (static_cast<__uint128_t>(hi) << 64) | lo;
}
#endif

} // namespace pfs

