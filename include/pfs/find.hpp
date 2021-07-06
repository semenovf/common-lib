////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2021 Vladislav Trifochkin
//
// This file is part of [common-lib](https://github.com/semenovf/common-lib) library.
//
// Changelog:
//      2017.01.26 Initial version of pfs/algo/find.hpp
//      2021.07.06 Refactored.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <utility>

namespace pfs {

/**
 * Find occurrence of sequence (needle) specified
 * by pair @a needle_begin and @a needle_end in sequence (haystack) specified
 * by pair @a haystack_begin and @a haystack_end and return pair of iterators
 * indicating first and last position of needle in haystack.
 */
template <typename ForwardIt1, typename ForwardIt2>
std::pair<ForwardIt1, ForwardIt1> find (ForwardIt1 haystack_begin
    , ForwardIt1 haystack_end
    , ForwardIt2 needle_begin
    , ForwardIt2 needle_end)
{
    if (haystack_begin == haystack_end)
        return std::make_pair(haystack_end, haystack_end);

    if (needle_begin == needle_end)
        return std::make_pair(haystack_end, haystack_end);

    while (haystack_begin != haystack_end) {
        if (*haystack_begin == *needle_begin) {
            ForwardIt1 haystack_pos(haystack_begin);
            ForwardIt2 needle_pos(needle_begin);

            ++haystack_pos;
            ++needle_pos;

            while (haystack_pos != haystack_end && needle_pos != needle_end) {
                if (*haystack_pos != *needle_pos)
                    break;
                ++haystack_pos;
                ++needle_pos;
            }

            if (needle_pos == needle_end)
                return std::make_pair(haystack_begin, haystack_pos);
        }

        ++haystack_begin;
    }

    return std::make_pair(haystack_end, haystack_end);
}

// template <typename InputIt1, typename InputIt2>
// InputIt1 rfind (
//           InputIt1 haystack_begin
//         , InputIt1 haystack_end
//         , InputIt2 needle_begin
//         , InputIt2 needle_end)
// {
//     if (haystack_end < haystack_begin)
//         return InputIt1(haystack_end);
//
//     if (needle_end <= needle_begin)
//         return InputIt1(haystack_end);
//
//     InputIt1 hend(haystack_end);
//
//     while (hend != haystack_begin) {
//
//         InputIt1 it(hend);
//         InputIt2 it1(needle_end);
//
//         do {
//             --it;
//             --it1;
//
//             if (*it != *it1)
//                 break;
//         } while (it != haystack_begin && it1 != needle_begin);
//
//         if (*it == *it1 && it1 == needle_begin)
//             return it;
//
//         --hend;
//     }
//
//     return InputIt1(haystack_end);
// }

} // namespace pfs
