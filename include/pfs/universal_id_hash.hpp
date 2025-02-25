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
        std::uint64_t hi = high(u);
        std::uint64_t lo = low(u);

        std::hash<std::uint64_t> hasher;
        auto result = hasher(hi);
        result ^= hasher(lo) + 0x9e3779b9 + (result << 6) + (result >> 2);
        return result;
    }
};

} // namespace std
