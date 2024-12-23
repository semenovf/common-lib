////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2021.11.09 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "universal_id.hpp"
#include <functional>

namespace std {

template<>
struct hash<pfs::universal_id>
{
    // See https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
    std::size_t operator () (pfs::universal_id const & u) const noexcept
    {
#if defined(PFS__UNIVERSAL_ID_IMPL_UUIDV7)
        std::uint64_t hi = u.high();
        std::uint64_t lo = u.low();
#else // !PFS__UNIVERSAL_ID_IMPL_UUIDV7
#   ifdef ULIDUINT128
        std::uint64_t hi = u.u >> 64;
        std::uint64_t lo = static_cast<std::uint64_t>(u.u);
#   else
        std::uint64_t hi = static_cast<std::uint64_t>(u.u.data[15])
            | static_cast<std::uint64_t>(u.u.data[14]) << 8
            | static_cast<std::uint64_t>(u.u.data[13]) << 16
            | static_cast<std::uint64_t>(u.u.data[12]) << 24
            | static_cast<std::uint64_t>(u.u.data[11]) << 32
            | static_cast<std::uint64_t>(u.u.data[10]) << 40
            | static_cast<std::uint64_t>(u.u.data[9])  << 48
            | static_cast<std::uint64_t>(u.u.data[8])  << 56;

        std::uint64_t lo = static_cast<std::uint64_t>(u.u.data[7])
            | static_cast<std::uint64_t>(u.u.data[6]) << 8
            | static_cast<std::uint64_t>(u.u.data[5]) << 16
            | static_cast<std::uint64_t>(u.u.data[4]) << 24
            | static_cast<std::uint64_t>(u.u.data[3]) << 32
            | static_cast<std::uint64_t>(u.u.data[2]) << 40
            | static_cast<std::uint64_t>(u.u.data[1]) << 48
            | static_cast<std::uint64_t>(u.u.data[0]) << 56;
#   endif
#endif // PFS__UNIVERSAL_ID_IMPL_UUIDV7
        std::hash<std::uint64_t> hasher;
        auto result = hasher(hi);
        result ^= hasher(lo) + 0x9e3779b9 + (result << 6) + (result >> 2);
        return result;
    }
};

} // namespace std
