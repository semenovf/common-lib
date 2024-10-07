////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2024 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2024.09.16 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <cstdint>
#include <functional>

namespace pfs {

// See https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x

template <class T>
inline void hash_combine_2 (std::size_t & result, T const & v)
{
    std::hash<T> hasher;
    result ^= hasher(v) + 0x9e3779b9 + (result << 6) + (result >> 2);
}

inline void hash_combine (std::size_t & seed)
{}

template <typename T, typename... Rest>
inline void hash_combine(std::size_t & result, T const & v, Rest... rest)
{
    std::hash<T> hasher;
    result ^= hasher(v) + 0x9e3779b9 + (result << 6) + (result >> 2);
    hash_combine(result, rest...);
}

} // namespace pfs