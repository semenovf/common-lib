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

    /**
 * Searches for the first occurrence of the sequence of elements
 * [@a s_first, @a s_last) in the range [@a first, @a last).
 *
 * @note Implemented as simple wrapper over @c std::search.
 */
template <typename HaystackIt, typename NeedleIt, typename BinaryPredicate>
inline HaystackIt search (HaystackIt first, HaystackIt last
    , NeedleIt s_first, NeedleIt s_last
    , BinaryPredicate predicate)
{
    return std::search(first, last, s_first, s_last, std::move(predicate));
}

/**
 * Searches for the first occurrence of the sequence of elements
 * [@a s_first, @a s_last) in the range [@a first, @a last). Sequence bitween
 * characters @a begin_ignore_char and @a end_ignore_char will be excluded.
 */
template <typename HaystackIt, typename NeedleIt, typename BinaryPredicate>
inline HaystackIt search (HaystackIt first, HaystackIt last
    , NeedleIt s_first, NeedleIt s_last
    , char_t begin_ignore_char, char_t end_ignore_char
    , BinaryPredicate predicate)
{
    bool ignore = false;

    while (first != last) {
        if (ignore) {
            if (*first == end_ignore_char) {
                ignore = false;
            }
        } else {
            HaystackIt pos = first;

            for (NeedleIt s_it = s_first; ; ++pos, ++s_it) {
                if (s_it == s_last)
                    return first;

                if (pos == last)
                    return last;

                if (*pos == begin_ignore_char) {
                    ignore = true;
                    first = pos;
                    break;
                }

                if (!predicate(*pos, *s_it))
                    break;
            }
        }

        ++first;
    }

    return last;
}

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
template <typename HaystackIt, typename NeedleIt>
void search_all (HaystackIt first, HaystackIt last, NeedleIt s_first, NeedleIt s_last
    , bool ignore_case, std::function<void(match_item const &)> on_matched)
{
    if (first == last)
        return;

    if (s_first == s_last)
        return;

    auto s_dist   = NeedleIt::distance_unsafe(s_first, s_last);
    auto s_cp_len = s_dist.first;
    auto s_cu_len = s_dist.second;

    auto predicate = ignore_case
        ? [] (pfs::unicode::char_t a, pfs::unicode::char_t b)->bool { return pfs::unicode::to_lower(a) == pfs::unicode::to_lower(b); }
        : [] (pfs::unicode::char_t a, pfs::unicode::char_t b)->bool { return a == b; };

    match_item m {0, 0, 0, 0};

    auto prev_pos = first;
    auto pos = search(prev_pos, last, s_first, s_last, predicate);

    while (pos != last) {
        auto res = HaystackIt::distance_unsafe(prev_pos, pos);

        m.cp_first += res.first;
        m.cu_first += res.second;

        m.cp_last += res.first + s_cp_len;
        m.cu_last += res.second + s_cu_len;

        NeedleIt::advance_unsafe(pos, s_cp_len);

        on_matched(m);

        prev_pos = pos;
        m.cp_first += s_cp_len;
        m.cu_first += s_cu_len;

        pos = search(prev_pos, last, s_first, s_last, predicate);
    }
}

template <typename HaystackIt, typename NeedleIt>
void search_all (HaystackIt first, HaystackIt last, NeedleIt s_first, NeedleIt s_last
    , bool ignore_case, char_t begin_ignore_char, char_t end_ignore_char
    , std::function<void(match_item const &)> on_matched)
{
    if (first == last)
        return;

    if (s_first == s_last)
        return;

    auto s_dist   = NeedleIt::distance_unsafe(s_first, s_last);
    auto s_cp_len = s_dist.first;
    auto s_cu_len = s_dist.second;

    auto predicate = ignore_case
        ? [] (pfs::unicode::char_t a, pfs::unicode::char_t b)->bool { return pfs::unicode::to_lower(a) == pfs::unicode::to_lower(b); }
        : [] (pfs::unicode::char_t a, pfs::unicode::char_t b)->bool { return a == b; };

    match_item m {0, 0, 0, 0};

    auto prev_pos = first;
    auto pos = search(prev_pos, last, s_first, s_last, begin_ignore_char
        , end_ignore_char, predicate);

    while (pos != last) {
        auto res = HaystackIt::distance_unsafe(prev_pos, pos);

        m.cp_first += res.first;
        m.cu_first += res.second;

        m.cp_last += res.first + s_cp_len;
        m.cu_last += res.second + s_cu_len;

        NeedleIt::advance_unsafe(pos, s_cp_len);

        on_matched(m);

        prev_pos = pos;
        m.cp_first += s_cp_len;
        m.cu_first += s_cu_len;

        pos = search(prev_pos, last, s_first, s_last, begin_ignore_char
            , end_ignore_char, predicate);
    }
}

}} // namespace pfs::unicode

