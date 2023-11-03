////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2023 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2019.??.?? Initial version
////////////////////////////////////////////////////////////////////////////////
#pragma once

namespace pfs {

// Base class (used by inheritor for ADL)
struct compare_operations {};

template <typename T, typename U>
inline bool operator != (T const & a, U const & b)
{
    return !(a == b);
}

template <typename T, typename U>
inline bool operator <= (T const & a, U const & b)
{
    return !(b < a);
}

template <typename T, typename U>
inline bool operator > (T const & a, U const & b)
{
    return b < a;
}

template <typename T, typename U>
inline bool operator >= (T const & a, U const & b)
{
    return !(a < b);
}

/**
 */
template <typename ForwardIt1, typename ForwardIt2, typename EndSeq2, typename Compare>
int compare_until (ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 first2, EndSeq2 eos2, Compare comp)
{
    while (first1 != last1 && !eos2(first2)) {
        if (comp(*first1, *first2))
            return -1;

        if (comp(*first2, *first1))
            return 1;

        ++first1;
        ++first2;
    }

    if (first1 == last1 && !eos2(first2))
        return -1;

    if (first1 != last1 && eos2(first2))
        return 1;

    return 0;
}

/**
 */
template <typename ForwardIt1, typename ForwardIt2, typename EndSeq1, typename EndSeq2, typename Compare>
int compare_until (ForwardIt1 first1, EndSeq1 eos1, ForwardIt2 first2, EndSeq2 eos2, Compare comp)
{
    while (!eos1(first1) && !eos2(first2)) {
        if (comp(*first1, *first2))
            return -1;

        if (comp(*first2, *first1))
            return 1;

        ++first1;
        ++first2;
    }

    if (eos1(first1) && !eos2(first2))
        return -1;

    if (!eos1(first1) && eos2(first2))
        return 1;

    return 0;
}

} // namespace pfs
