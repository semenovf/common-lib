////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020-2023 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2023.04.12 Initial version.
//
// See:
//      1. https://en.wikipedia.org/wiki/Levenshtein_distance
//      2. https://en.wikipedia.org/wiki/Levenshtein_distance#Iterative_with_two_matrix_rows
//      3. https://github.com/wooorm/levenshtein.c
//      4. https://github.com/hiddentao/fast-levenshtein
//
// Notes
//      * Fast implementation inspired by [2], [3] and [4].
//
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "error.hpp"
#include "string_view.hpp"
#include "type_traits.hpp"
#include <algorithm>
#include <iterator>
#include <cstring>
#include <string>
#include <vector>

namespace pfs {

enum class levenshtein_distance_algo {
      wagner_fischer
    , fast

    // TODO Implement
    , damerau // Damerau–Levenshtein
};

template <typename Char>
struct default_equality_comparator
{
    inline bool operator ()  (Char a, Char b) const noexcept { return a == b; }
};

// Wagner–Fischer algorithm implementation.
// Iterative with full matrix.

template <typename ForwardIter
    , typename EqualityComparator = default_equality_comparator<pointer_dereference_t<ForwardIter>>
    , typename SizeType = std::size_t>
SizeType levenshtein_distance_wf (ForwardIter xbegin, ForwardIter xend
    , ForwardIter ybegin, ForwardIter yend)
{
    using matrix_type = std::vector<std::vector<SizeType>>;

    auto xlen = std::distance(xbegin, xend);
    auto ylen = std::distance(ybegin, yend);

    if (xbegin == ybegin && xend == yend)
        return 0;

    if (xlen == 0)
        return ylen;

    if (ylen == 0)
        return xlen;

    matrix_type d (xlen + 1);

    for (SizeType i = 0; i <= xlen; i++)
        d[i].resize(ylen + 1);

    for (SizeType i = 0; i <= xlen; i++)
        d[i][0] = i;

    // NOTE Can be replace with std::iota call
    // std::iota(d[0].begin(), d.[0].end(), 0);
    for (SizeType j = 0; j <= ylen; j++)
        d[0][j] = j;

    auto xcursor = xbegin;
    EqualityComparator eq;

    for (SizeType i = 1; i <= xlen; i++) {
        auto ycursor = ybegin;

        for (SizeType j = 1; j <= ylen; j++) {
            //int subst_cost = (x[i - 1] == y[j - 1]) ? 0 : 1;
            int subst_cost = eq(*xcursor, *ycursor) ? 0 : 1;

            d[i][j] = (std::min)((std::min)(d[i - 1][j] + 1, d[i][j - 1] + 1)
                , d[i - 1][j - 1] + subst_cost);

            ++ycursor;
        }

        ++xcursor;
    }

    return d[xlen][ylen];
}

template <typename ForwardIter
    , typename EqualityComparator = default_equality_comparator<pointer_dereference_t<ForwardIter>>
    , typename SizeType = std::size_t>
SizeType levenshtein_distance_fast (ForwardIter xbegin, ForwardIter xend
    , ForwardIter ybegin, ForwardIter yend)
{
    if (xbegin == ybegin && xend == yend)
        return 0;

    auto xlen = std::distance(xbegin, xend);
    auto ylen = std::distance(ybegin, yend);

    if (xlen == 0)
        return ylen;

    if (ylen == 0)
        return xlen;

    EqualityComparator eq;
    auto xpos = xbegin;
    auto ypos = ybegin;

    //--------------------------------------------------------------------------
    // Inspired by implementation [4]
    //--------------------------------------------------------------------------
#if 0
    std::vector<SizeType> cache(ylen + 1);
    SizeType result = 0;

    for (SizeType i = 0; i <= ylen; ++i)
        cache[i] = i;

    for (SizeType i = 0; i < xlen; ++i) {
        result = i + 1;
        ypos = ybegin;

        for (SizeType j = 0; j < ylen; ++j) {
            result = cache[j] + (eq(*xpos, *ypos) ? 0 : 1);

            auto n = result + 1;

            if (result > n)
                result = n;

            n = cache[j + 1] + 1;

            if (result > n)
                result = n;

            cache[j] = result;

            ++ypos;
        }

        cache[ylen] = result;
        ++xpos;
    }

    return result;
#endif

    //--------------------------------------------------------------------------
    // Inspired by implementation [3]
    //--------------------------------------------------------------------------
#if 1
    std::vector<SizeType> cache(xlen);
    SizeType result = 0;

    for (SizeType i = 0; i < xlen; ++i)
        cache[i] = i + 1;

    for (SizeType j = 0; j < ylen; ++j) {
        auto m = j;
        result = m;
        xpos = xbegin;

        for (SizeType i = 0; i < xlen; ++i) {
            auto n = eq(*ypos, *xpos) ? m : m + 1;
            m = cache[i];

            result = m > result
                ? n > result ? result + 1 : n
                : n > m ? m + 1 : n;

            cache[i] = result;
            ++xpos;
        }

        ++ypos;
    }

  return result;
#endif

//--------------------------------------------------------------------------
// Inspired by implementation [2]
//--------------------------------------------------------------------------
#if 0
    std::vector<SizeType> v0(ylen + 1);
    std::vector<SizeType> v1(ylen + 1);

    for (SizeType i = 0; i <= ylen; i++)
        v0[i] = i;

    for (SizeType i = 0; i < xlen; i++) {
        v1[0] = i + 1;
        ypos = ybegin;

        for (SizeType j = 0; j < ylen; j++) {
            auto deletionCost = v0[j + 1] + 1;
            auto insertionCost = v1[j] + 1;
            auto substitutionCost = eq(*xpos, *ypos) ? v0[j] : v0[j] + 1;
            v1[j + 1] = (std::min)((std::min)(deletionCost, insertionCost)
                , substitutionCost);
            ++ypos;
        }

        std::swap(v0, v1);
        ++xpos;
    }

    return v0[ylen];
#endif
}

template <typename ForwardIter
    , typename EqualityComparator = default_equality_comparator<pointer_dereference_t<ForwardIter>>
    , typename SizeType = std::size_t>
SizeType levenshtein_distance (ForwardIter xbegin, ForwardIter xend
    , ForwardIter ybegin, ForwardIter yend
    , levenshtein_distance_algo algo = levenshtein_distance_algo::fast)
{
    switch (algo) {
        case levenshtein_distance_algo::wagner_fischer:
            return levenshtein_distance_wf<ForwardIter, EqualityComparator, SizeType>(
                xbegin, xend, ybegin, yend);
        case levenshtein_distance_algo::fast:
        default:
            break;
    }

    return levenshtein_distance_fast<ForwardIter, EqualityComparator, SizeType>(
        xbegin, xend, ybegin, yend);
}

template <typename ForwardIter = char const *
    , typename EqualityComparator = default_equality_comparator<pointer_dereference_t<char const *>>
    , typename SizeType = std::size_t>
inline SizeType levenshtein_distance (char const * a, char const * b
    , levenshtein_distance_algo algo = levenshtein_distance_algo::fast)
{
    if (!a)
        throw error {std::make_error_code(std::errc::invalid_argument)};

    if (!b)
        throw error {std::make_error_code(std::errc::invalid_argument)};

    return levenshtein_distance<ForwardIter, EqualityComparator, SizeType>(
        ForwardIter{a}, ForwardIter{a + std::strlen(a)}, ForwardIter{b}
            , ForwardIter{b + std::strlen(b)}, algo);
}

template <typename ForwardIter = std::string::const_iterator
    , typename EqualityComparator = default_equality_comparator<pointer_dereference_t<std::string::const_iterator>>
    , typename SizeType = std::size_t>
inline SizeType levenshtein_distance (std::string const & a, std::string const & b
    , levenshtein_distance_algo algo = levenshtein_distance_algo::fast)
{
    return levenshtein_distance(ForwardIter{a.cbegin()}, ForwardIter{a.cend()}
        , ForwardIter{b.cbegin()}, ForwardIter{b.cend()}, algo);
}

template <typename ForwardIter = string_view::const_iterator
    , typename EqualityComparator = default_equality_comparator<pointer_dereference_t<string_view::const_iterator>>
    , typename SizeType = std::size_t>
inline SizeType levenshtein_distance (string_view const & a, string_view const & b
        , levenshtein_distance_algo algo = levenshtein_distance_algo::fast)
{
    return levenshtein_distance(ForwardIter{a.cbegin()}, ForwardIter{a.cend()}
        , ForwardIter{b.cbegin()}, ForwardIter{b.cend()}, algo);
}

template <typename ForwardIter
    , typename EqualityComparator
    , typename SizeType
    , levenshtein_distance_algo Algo = levenshtein_distance_algo::fast>
struct levenshtein_distance_env;

template <typename ForwardIter
    , typename EqualityComparator
    , typename SizeType>
struct levenshtein_distance_env<ForwardIter, EqualityComparator, SizeType, levenshtein_distance_algo::fast>
{
    inline SizeType operator () (ForwardIter xbegin, ForwardIter xend
        , ForwardIter ybegin, ForwardIter yend) const
    {
        return levenshtein_distance_fast<ForwardIter, EqualityComparator, SizeType>(
            xbegin, xend, ybegin, yend);
    }
};

} // namespace pfs
