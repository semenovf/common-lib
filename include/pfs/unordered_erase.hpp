////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2024 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2024.01.18 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <vector>

namespace pfs {

// https://stackoverflow.com/questions/31115718/o1-deletion-in-an-unordered-vector

template <typename T, typename Allocator = std::allocator<T>>
void unordered_erase (std::vector<T, Allocator> & v, typename std::vector<T, Allocator>::iterator pos)
{
    *pos = std::move(v.back());
    v.pop_back();
}

template <typename T, typename Allocator = std::allocator<T>>
void unordered_erase (std::vector<T, Allocator> & v, typename std::vector<T, Allocator>::size_type index)
{
    v[index] = std::move(v.back());
    v.pop_back();
}

} // namespace pfs

