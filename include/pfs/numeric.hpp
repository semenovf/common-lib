////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of [common-lib](https://github.com/semenovf/common-lib) library.
//
// Changelog:
//      2020.03.28 Initial version (inspired from https://github.com/semenovf/pfs)
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <numeric>

namespace pfs {

//
// The idea and implementation is from:
// https://stackoverflow.com/questions/1903954/is-there-a-standard-sign-function-signum-sgn-in-c-c
//
template <typename T>
int sign_of (T value)
{
    // Avoid repeated construction
    T zero(0);
    return (zero < value) - (value < zero);
}

#if __cplusplus >= 201703L

template <typename M, typename N>
constexpr std::common_type_t<M, N> gcd (M m, N n)
{
    return std::gcd(m, n);
}

template <typename M, typename N>
constexpr std::common_type_t<M, N> lcm (M m, N n)
{
    return std::lcm(m, n);
}

#else

/**
 * @brief Calculates Greatest Common Divisor. Based on Knuth 4.5.2. algorithm B.
 */
template <typename M, typename N>
typename std::common_type<M, N>::type gcd (M a, N b) noexcept
{
    // Avoid repeated construction
    typename std::common_type<M, N>::type zero(0);

    if (b == zero)
        return a;

    if (a == zero)
        return b;

    if (a < zero)
        a = -a;

    if (b < zero)
        b = -b;

    int k = 0;

    while (!((a & 1) || (b & 1))) { //B1
        a >>= 1;
        b >>= 1;
        ++k;
    }

    typename std::common_type<M, N>::type t = (a & 1) ? -b : a; //B2

    while (a != b) {
        while (t && ((t & 1) ^ 1)) t >>= 1;  //B3 & B4

        if (t < zero) b = -t; //B5
        else a = t;

        t = a - b;  //B6
    }

    return a << k;
}

/**
 * @brief Calculates Lowest (Least) Common Multiple.
 */
template <typename M, typename N>
inline typename std::common_type<M, N>::type lcm (M a, N b) noexcept
{
    // Avoid repeated construction
    typename std::common_type<M, N>::type zero(0);
    typename std::common_type<M, N>::type g = gcd(a, b);
    return (g != zero) ? (a / g * b) : zero;
}

#endif

} // namespace pfs

