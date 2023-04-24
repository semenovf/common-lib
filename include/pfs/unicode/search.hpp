////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020-2023 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2023.04.24 Initial version
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "char.hpp"
#include "traits.hpp"
#include <algorithm>
#include <functional>

namespace pfs {
namespace unicode {

struct match_item
{
    int cp_first; // first position in code points of the matched subrange
    int cp_last;  // last position in code points of the matched subrange (exclusive)
    int cu_first; // first position in code units of the matched subrange
    int cu_last;  // last position in code units of the matched subrange (exclusive)
};

/**
 * Searches for the all occurrences of the sequence of elements
 * [@a s_first, @a s_last) in the range [@a first, @a last).
 */
template <typename UtfIt>
void search_all (UtfIt first, UtfIt last, UtfIt s_first, UtfIt s_last
    , bool ignore_case, std::function<void(match_item const &)> && f)
{
    if (first == last)
        return;

    if (s_first == s_last)
        return;

    auto s_dist   = UtfIt::distance_unsafe(s_first, s_last);
    auto s_cp_len = s_dist.first;
    auto s_cu_len = s_dist.second;

    auto predicate = ignore_case
        ? [] (pfs::unicode::char_t a, pfs::unicode::char_t b)->bool { return pfs::unicode::to_lower(a) == pfs::unicode::to_lower(b); }
        : [] (pfs::unicode::char_t a, pfs::unicode::char_t b)->bool { return a == b; };

    match_item m {0, 0, 0, 0};

    auto prev_pos = first;
    auto pos = std::search(prev_pos, last, s_first, s_last, predicate);

    while (pos != last) {
        auto res = UtfIt::distance_unsafe(prev_pos, pos);

        m.cp_first += res.first;
        m.cu_first += res.second;

        m.cp_last += res.first + s_cp_len;
        m.cu_last += res.second + s_cu_len;

        UtfIt::advance_unsafe(pos, s_cp_len);

        f(m);

        prev_pos = pos;
        m.cp_first += s_cp_len;
        m.cu_first += s_cu_len;

        pos = std::search(prev_pos, last, s_first, s_last, predicate);
    }
}

}} // namespace pfs::unicode

