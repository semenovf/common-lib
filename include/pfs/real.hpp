////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2024 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2024.10.07 Initial version (inspired from https://github.com/semenovf/pfs)
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "optional.hpp"
#include <cctype>
#include <cstdint>
#include <limits>
#include <type_traits>

/*
 * Grammars:
 *
 * [1] number = integer / real / rational
 *
 * [2] integer = integral-part
 *
 * [3] real = [integral-part] POINT fract-part [exp-part]
 *          / integral-part POINT [exp-part]
 *          / integral-part exp-part
 *          / [sign] (N | n ) (A | a ) (N | n )
 *          / [sign] (I | i ) (N | n ) (F | f ) [(I | i ) (N | n ) (I | i ) (T | t ) (Y | y )]
 *
 * [4] rational = integral-part
 *          /  integral-part POINT [ fract-part ]
 *          /  numinator *ws '/' *ws denominator
 *
 * integral-part = *ws [ sign ] +DIGIT
 *
 * fract-part    = +DIGIT
 *
 * numinator     = integral-part
 *
 * denominator   = +DIGIT
 *
 * sign = '+' / '-'
 *
 * POINT = '.' ; or user-defined
 *
 * NON_ZERO_DIGIT = "1" / "2" / ... / "9" / ...  ; depends on radix
 *
 * DIGIT          = "0" / 1" / "2" / ... / "9" / ... ; depends on radix
 *
 * ws             = <whitespace>
 *
 */

//
// Hack to get locale dependent decimal point char (spied at stackoverflow.com)
//
// setlocale(LC_NUMERIC, "C");
// char fchars[10];
// sprintf(fchars, "%f", 0.0f);
// char decimalPoint = fchars[1];
//

namespace pfs {

//
// Table giving binary powers of 10. Entry is 10^2^i.Used to convert decimal
// exponents into floating-point numbers.
//
template <typename FloatT>
struct powers_of_10;

template <>
struct powers_of_10<float>
{
    static float * values ()
    {
        static float a[] = {
            10.
          , 100.
          , 1.0e4
          , 1.0e8
          , 1.0e16
          , 1.0e32
        };

        return a;
    }
};

template <>
struct powers_of_10<double>
{
    static double * values ()
    {
        static double a[] = {
            10.
          , 100.
          , 1.0e4
          , 1.0e8
          , 1.0e16
          , 1.0e32
          , 1.0e64
          , 1.0e128
          , 1.0e256
        };

        return a;
    }
};

namespace {
    template <typename CharT>
    char to_ascii (CharT c);

    template <>
    inline char to_ascii<char> (char c)
    {
        return c >= 0 ? c : -1;
    }
}

template <typename RealT, typename CharIt>
bool advance_real (CharIt & pos, CharIt last, char decimal_point_char, RealT & result)
{
    //using char_type = typename std::remove_reference<decltype(*pos)>::type;
    using char_type = typename std::decay<decltype(*pos)>::type;

    static double frac1_powers[] = {1.0e0, 1.0e1, 1.0e2, 1.0e3, 1.0e4, 1.0e5
            , 1.0e6, 1.0e7, 1.0e8, 1.0e9};

    result = RealT{0};

    int sign = 1;

    if (pos == last)
        return false;

    auto p = pos;

    // Skip over any leading whitespace
    while (p != last && std::isspace(*p))
        ++p;

    if (p == last)
        return false;

    // Parse sign
    //==========================================================================
    if (*p == char_type('-')) {
        sign = -1;
        ++p;
    } else if (*p == char_type('+')) {
        ++p;
    }

    if (p == last)
        return false;

    // Parse NaN

    if (*p == 'N' || *p == 'n') {
        ++p;

        if ((p != last) && (*p == 'A' || *p == 'a')) {
            ++p;

            if ((p != last) && (*p == 'N' || *p == 'n')) {
                ++p;

                pos = p;
                result = std::numeric_limits<RealT>::quiet_NaN();

                if (sign < 0)
                    result = -result;

                return true;
            }
        }

        return false;
    }

    if (p == last)
        return false;

    // Parse +-Infinity

    if (*p == 'I' || *p == 'i') {
        ++p;

        if ((p != last) && (*p == 'N' || *p == 'n')) {
            ++p;

            if ((p != last) && (*p == 'F' || *p == 'f')) {
                ++p;

                if ((p != last) && (*p == 'I' || *p == 'i')) {
                    ++p;

                    if ((p != last) && (*p == 'N' || *p == 'n')) {
                        ++p;

                        if ((p != last) && (*p == 'I' || *p == 'i')) {
                            ++p;

                            if ((p != last) && (*p == 'T' || *p == 't')) {
                                ++p;

                                if ((p != last) && (*p == 'Y' || *p == 'y')) {
                                    ++p;

                                    pos = p;
                                    result = std::numeric_limits<RealT>::infinity();

                                    return true;
                                }
                            }
                        }
                    }
                } else {
                    pos = p;
                    result = std::numeric_limits<RealT>::infinity();

                    return true;
                }
            }
        }

        return false;
    }

    if (p == last)
        return false;

    if (*p == '0') {
        // Skip zeros
        while (p != last && *p == '0')
            ++p;

        pos = p;
    }

    // Zeros only is a valid result
    if (p == last)
        return pos == p;

    // Parse mantissa
    //==========================================================================
    // For double type mantissa has 18 significant digits,
    // so extra digits could be considered zeros (they can't affect the
    // result value). And represent this 18 digits as two integer fractions

    int mantissa_size = 0;
    int decimal_point_size = -1;
    std::int32_t frac1 = 0, frac2 = 0;

    for (char_type c = *p; p != last && mantissa_size < 9; ++p, ++mantissa_size, c = *p) {
        if (!std::isdigit(c)) {
            if (c == decimal_point_char) {
                if (decimal_point_size < 0) {
                    decimal_point_size = mantissa_size;
                    --mantissa_size;
                    continue;
                } else { // second decimal point
                    break;
                }
            } else {
                break;
            }
        }

        frac1 = 10 * frac1 + (to_ascii(c) - '0');
    }

    int frac1_power = 0;

    for (char_type c = *p; p != last && mantissa_size < 18; ++p, ++mantissa_size, c = *p) {
        if (!std::isdigit(c)) {
            if (c == decimal_point_char) {
                if (decimal_point_size < 0) {
                    decimal_point_size = mantissa_size;
                    --mantissa_size;
                    continue;
                } else { // second decimal point
                    break;
                }
            } else {
                break;
            }
        }

        frac2 = 10 * frac2 + (to_ascii(c) - '0');
        ++frac1_power;
    }

    while (p != last && std::isdigit(*p)) {
        ++mantissa_size;
        ++p;
    }

    if (decimal_point_size < 0) {
        decimal_point_size = mantissa_size;
    }
//    } else {
//        --mantissa_size; // One of the digits was the point
//    }

    int frac_exp = 0;

    if (mantissa_size > 18) {
        frac_exp = decimal_point_size - 18;
        mantissa_size = 18;
    } else {
        frac_exp = decimal_point_size - mantissa_size;
    }

    if (mantissa_size == 0) {
        // Error, no need to continue parsing
        return false; // Yes, return true, not false
    }

    // Commit result (but may be not complete)
    result = (static_cast<RealT>(frac1_powers[frac1_power]) * frac1) + frac2;
    pos = p;

    // Parse exponent

    int exp_sign = 1;
    int exp = 0;

    if ((*p == 'E') || (*p == 'e')) {
        auto saved_pos = p;

        ++p;

        if (p != last) {
            if (*p == '-') {
                exp_sign = -1;
                ++p;
            } else {
                if (*p == '+') {
                    ++p;
                }
            }

            if (p != last) {
                if (std::isdigit(*p)) {
                    while (p != last && isdigit(*p)) {
                        exp = exp * 10 + (to_ascii(*p) - '0');
                        ++p;
                    }
                    pos = p;
                } else {
                    p = saved_pos;
                }
            }
        }
    }

    if (exp_sign < 0) {
        exp = frac_exp - exp;
    } else {
        exp = frac_exp + exp;
    }

    // Generate a floating-point number that represents the exponent.
    // Do this by processing the exponent one bit at a time to combine
    // many powers of 2 of 10. Then combine the exponent with the
    // fraction.

    if (exp < 0) {
        exp_sign = -1;
        exp = -exp;
    } else {
        exp_sign = 1;
    }

    if (exp < std::numeric_limits<RealT>::min_exponent10) { // underflow
        return true; // Yes, return true, not false
    } else if (exp > std::numeric_limits<RealT>::max_exponent10) { // overflow
        if (sign < 0) {
            result = -std::numeric_limits<RealT>::infinity();
        } else {
            result = std::numeric_limits<RealT>::infinity();
        }

        return true; // Yes, return true, not false
    }

    RealT dbl_exp = RealT{1};

    for (RealT * d = powers_of_10<RealT>::values(); exp != 0; exp >>= 1, d += 1) {
        if (exp & 01) {
            dbl_exp *= *d;
        }
    }

    if (exp_sign < 0) {
        result /= dbl_exp;
    } else {
        result *= dbl_exp;
    }

    if (sign < 0)
        result = -result;

    return true;
}

template <typename RealT, typename CharIt>
inline optional<RealT> to_real (CharIt first, CharIt last, char decimal_point_char)
{
    RealT result {0};
    auto success = advance_real<RealT, CharIt>(first, last, decimal_point_char, result);
    return success ? make_optional(result) : nullopt;
}

template <typename RealT, typename CharIt>
inline optional<RealT> to_real (CharIt first, CharIt last)
{
    return to_real<RealT>(first, last, '.');
}

template <typename RealT>
inline optional<RealT> to_real (char const * first, std::size_t len, char decimal_point_char)
{
    return to_real<RealT>(first, first + len, decimal_point_char);
}

template <typename RealT>
inline optional<RealT> to_real (char const * first, std::size_t len)
{
    return to_real<RealT>(first, len, '.');
}

template <typename RealT>
inline optional<RealT> to_real (std::string const & s, char decimal_point_char)
{
    return to_real<RealT>(s.c_str(), s.size(), decimal_point_char);
}

template <typename RealT>
inline optional<RealT> to_real (std::string const & s)
{
    return to_real<RealT>(s, '.');
}

} // namespace pfs
