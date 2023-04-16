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
#include "i18n.hpp"
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

    , myers1999

    // https://en.wikipedia.org/wiki/Hamming_distance
    , hamming
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


////////////
// Original implementation in C from https://ceptord.net/20200815-Comparison.html
// (follow `benchmark.c` href)
//
// Myers' bit-parallel algorithm
//
// G. Myers. "A fast bit-vector algorithm for approximate string
// matching based on dynamic programming." Journal of the ACM, 1999.
//
// int8_t myers1999(char *s1, int8_t len1, char *s2, int8_t len2)
// {
//     uint64_t Peq[256];
//     uint64_t Eq, Xv, Xh, Ph, Mh, Pv, Mv, Last;
//     int8_t i;
//     int8_t Score = len2;
//
//     memset(Peq, 0, sizeof(Peq));
//
//     for (i = 0; i < len2; i++)
//         Peq[s2[i]] |= (uint64_t) 1 << i;
//
//     Mv = 0;
//     Pv = (uint64_t) -1;
//     Last = (uint64_t) 1 << (len2 - 1);
//
//     for (i = 0; i < len1; i++) {
//         Eq = Peq[s1[i]];
//
//         Xv = Eq | Mv;
//         Xh = (((Eq & Pv) + Pv) ^ Pv) | Eq;
//
//         Ph = Mv | ~ (Xh | Pv);
//         Mh = Pv & Xh;
//
//         if (Ph & Last) Score++;
//         if (Mh & Last) Score--;
//
//         Ph = (Ph << 1) | 1;
//         Mh = (Mh << 1);
//
//         Pv = Mh | ~ (Xv | Ph);
//         Mv = Ph & Xv;
//     }
//     return Score;
// }

//
// Limitations:
//      1. the maximum length of the each sequence must be less than or
//         equal to 32;
//      2. supports only Plane Unicode characters (https://en.wikipedia.org/wiki/Plane_(Unicode));
//      3. equality сomparator is not used.
//
template <typename ForwardIter
    , typename EqualityComparator = default_equality_comparator<pointer_dereference_t<ForwardIter>>
    , typename SizeType = std::size_t>
SizeType levenshtein_distance_myers1999 (ForwardIter xbegin, ForwardIter xend
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

    if (xlen > 32) {
        throw error {
              std::make_error_code(std::errc::invalid_argument)
            , tr::f_("length of the sequence must be less than or equal to 32")
        };
    }

    if (ylen > 32) {
        throw error {
              std::make_error_code(std::errc::invalid_argument)
            , tr::f_("length of the sequence must be less than or equal to 32")
        };
    }

    std::uint64_t Peq[0x10000];

    std::memset(Peq, 0, sizeof(Peq));

    std::int8_t i = 0;

    for (auto ypos = ybegin; ypos != yend; ++ypos) {
        auto index = static_cast<std::uint64_t>(*ypos);

        if (index >= 0x10000) {
            throw error {
                  std::make_error_code(std::errc::invalid_argument)
                , tr::f_("only Plane Unicode characters supported by Myers1999 algorithm")
            };
        }

        Peq[index] |= static_cast<std::uint64_t>(1) << i++;
    }

    std::uint64_t Mv = 0;
    std::uint64_t Pv = static_cast<std::uint64_t>(-1);
    std::uint64_t Last = static_cast<std::uint64_t>(1) << (ylen - 1);
    std::int8_t Score = static_cast<std::int8_t>(ylen); //len2;

    for (auto xpos = xbegin; xpos != xend; ++xpos) {
        auto index = static_cast<std::uint64_t>(*xpos);

        if (index >= 0x10000) {
            throw error {
                  std::make_error_code(std::errc::invalid_argument)
                , tr::f_("only Plane Unicode characters supported by Myers1999 algorithm")
            };
        }

        std::uint64_t Eq = Peq[index];

        std::uint64_t Xv = Eq | Mv;
        std::uint64_t Xh = (((Eq & Pv) + Pv) ^ Pv) | Eq;

        std::uint64_t Ph = Mv | ~ (Xh | Pv);
        std::uint64_t Mh = Pv & Xh;

        if (Ph & Last)
            Score++;

        if (Mh & Last)
            Score--;

        Ph = (Ph << 1) | 1;
        Mh = (Mh << 1);

        Pv = Mh | ~ (Xv | Ph);
        Mv = Ph & Xv;
    }

    return Score;
}

//
// Limitations:
//      1. only equal sized (or empty) sequences are applicable.
//
template <typename ForwardIter
    , typename EqualityComparator = default_equality_comparator<pointer_dereference_t<ForwardIter>>
    , typename SizeType = std::size_t>
SizeType levenshtein_distance_hamming (ForwardIter xbegin, ForwardIter xend
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

    if (xlen != ylen) {
        throw error {
              std::make_error_code(std::errc::invalid_argument)
            , tr::f_("only equal sized (or empty) sequences are applicable for Hamming algorithm")
        };
    }

    EqualityComparator eq;
    auto xpos = xbegin;
    auto ypos = ybegin;
    SizeType counter = 0;

    for (; xpos != xend; ++xpos, ++ypos) {
        if (!eq(*xpos, *ypos))
            counter++;
    }

    return counter;
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
        case levenshtein_distance_algo::myers1999:
            return levenshtein_distance_myers1999<ForwardIter, EqualityComparator, SizeType>(
                xbegin, xend, ybegin, yend);
        case levenshtein_distance_algo::hamming:
            return levenshtein_distance_hamming<ForwardIter, EqualityComparator, SizeType>(
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
