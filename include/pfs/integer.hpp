////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020-2022 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2022.09.07 Initial version (inspired from https://github.com/semenovf/pfs)
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "error.hpp"
#include <limits>
#include <system_error>
#include <type_traits>
#include <cctype>

/*
 * Grammars:
 *
 * integer = *ws [ sign ] +DIGIT
 *
 * sign = '+' / '-'
 *
 * DIGIT          = "0" / 1" / "2" / ... / "9" / ... ; depends on radix
 *
 * ws             = <whitespace>
 *
 */

namespace pfs {

/**
 * @return Base-@a radix Digit converted from character, or -1 if conversion
 *      is impossible.
 */
template <typename CharIt>
int parse_digit (CharIt pos, int radix)
{
    //typedef typename iterator_traits<CharIt>::value_type value_type;
    char c = *pos;
    int digit = 0;

    if (!std::isprint(c))
        return -1;

    if (std::isdigit(c))
        digit = c - '0';
    else if (std::islower(c))
        digit = c - 'a' + 10;
    else if (std::isupper(c))
        digit = c - 'A' + 10;
    else
        return -1;

    if (digit >= radix)
        return -1;

    return digit;
}

struct sign_radix_result
{
    int radix;
    int sign;
    bool digits_found;
    std::errc ec;
};

/**
 * Parses sign and radix if @a radix equals to zero.
 */
template <typename CharIt>
sign_radix_result parse_sign_radix (CharIt & pos, CharIt last, int radix)
{
    sign_radix_result result {radix, 1, false, std::errc{}};
    sign_radix_result invalid_result {0, 0, false, std::errc::invalid_argument};

    // Bad radix
    if (radix != 0 && (radix < 2 || radix > 36))
        return invalid_result;

    // Empty sequence
    if (pos == last)
        return invalid_result;

    // Skip white spaces
    while (pos != last && isspace(*pos))
        ++pos;

    // No digits seen
    if (pos == last)
        return invalid_result;

    char c = *pos;

    // Sign
    if (c == '+' || c == '-') {
        ++pos;

        // No digits seen
        if (pos == last)
            return invalid_result;

        if (c == '-')
            result.sign = -1;
    }

    // Determine radix
    if (radix == 0 || radix == 16) {
        CharIt nextpos = pos;
        ++nextpos;

        if (nextpos != last) {
            if (*pos == '0'
                    && (*nextpos == 'x'
                        || *nextpos == 'X')) {
                result.radix = 16;
                ++pos;
                ++pos;

                // No digits seen
                if (pos == last)
                    return invalid_result;
            }
        }
    }

    // Determine radix (continue)
    if (radix == 0 || radix == 8) {
        if (*pos == '0') {
            result.radix = 8;
            ++pos;
            result.digits_found = true;
        }
    }

    if (radix == 0)
        result.radix = 10;

    return result;
}

template <typename CharIt>
struct parse_integer_result
{
    CharIt ptr;
    std::errc ec;
};

/**
 * @return On success, returns a value of type @c from_chars_result such that
 *         @c ptr points at the first character not matching the pattern, or has
 *         the value equal to @a last if all characters match and @c ec is
 *         value-initialized.
 *
 *         If there is no pattern match, returns a value of type
 *         @c parse_integer_result such that @c ptr equals @a first and @c ec
 *         equals @c std::errc::invalid_argument. @a value is unmodified.
 *
 *         If the pattern was matched, but the parsed value is not in the range
 *         representable by the type of value, returns value of type
 *         @c parse_integer_result such that @c ec equals @c std::errc::result_out_of_range
 *         and @c ptr points at the first character not matching the pattern.
 *         @a value is unmodified.
 */
template <typename IntT, typename CharIt
    , typename std::enable_if<std::is_integral<IntT>::value, int>::type = 0>
parse_integer_result<CharIt>
parse_integer (CharIt first, CharIt last, IntT & value, int radix = 10)
{
    CharIt pos = first;
    auto sign_radix_res = parse_sign_radix(pos, last, radix);

    if (sign_radix_res.ec != std::errc{})
        return parse_integer_result<CharIt>{first, std::errc::invalid_argument};

    // Source has sign but parsing for unsigned integer
    if (std::is_unsigned<IntT>::value && sign_radix_res.sign < 0)
        return parse_integer_result<CharIt>{first, std::errc::invalid_argument};

    IntT result_value = 0;
    IntT cutoff_value = 0;
    IntT cutoff_limit = 0;

    if (std::is_unsigned<IntT>::value) {
        cutoff_value = std::numeric_limits<IntT>::max() / radix;
        cutoff_limit = std::numeric_limits<IntT>::max() % radix;
    } else {
        cutoff_value = std::numeric_limits<IntT>::min() / radix;
        cutoff_limit = std::numeric_limits<IntT>::min() % radix;

        cutoff_value *= -1;
        cutoff_limit *= -1;

        if (sign_radix_res.sign > 0)
            --cutoff_limit;
    }

    for (; pos != last; ++pos) {
        int digit = parse_digit<CharIt>(pos, radix);

        // Finish
        if (digit < 0)
            break;

        if (result_value < cutoff_value
                || (result_value == cutoff_value && static_cast<std::uintmax_t>(digit) <= cutoff_limit)) {
            result_value *= radix;
            result_value += digit;
        } else {
            break;
        }

        sign_radix_res.digits_found = true;
    }

    // Value is not in the range
    if (pos != last) {
        if (parse_digit<CharIt>(pos, radix) >= 0)
            return parse_integer_result<CharIt>{pos, std::errc::result_out_of_range};
    }

    result_value *= sign_radix_res.sign;
    value = result_value;
    return parse_integer_result<CharIt>{pos, std::errc{}};
}

template <typename IntT, typename CharIt
    , typename std::enable_if<std::is_integral<IntT>::value, int>::type = 0>
inline IntT
to_integer (CharIt first, CharIt last, int radix, std::error_code & ec)
{
    IntT value = 0;
    auto res = parse_integer<IntT, CharIt>(first, last, value, radix);

    if (res.ec != std::errc{}) {
        ec = make_error_code(res.ec);
        return 0;
    }

    if (res.ptr != last) {
        ec = make_error_code(std::errc::invalid_argument);
        return 0;
    }

    return value;
}

template <typename IntT, typename CharIt
    , typename std::enable_if<std::is_integral<IntT>::value, int>::type = 0>
inline IntT
to_integer (CharIt first, CharIt last, std::error_code & ec)
{
    return to_integer<IntT, CharIt>(first, last, 10, ec);
}

template <typename IntT, typename CharIt
    , typename std::enable_if<std::is_integral<IntT>::value, int>::type = 0>
inline IntT
to_integer (CharIt first, CharIt last, int radix = 10)
{
    std::error_code ec;
    auto n = to_integer<IntT, CharIt>(first, last, radix, ec);

    if (ec)
        throw error{ec, "bad integer string representation"};

    return n;
}

template <typename IntT, typename CharIt
    , typename std::enable_if<std::is_integral<IntT>::value, int>::type = 0>
inline IntT
to_integer (CharIt first, CharIt last, IntT min_value, IntT max_value, int radix, std::error_code & ec)
{
    auto n = to_integer<IntT, CharIt>(first, last, radix, ec);

    if (!ec) {
        if (n < min_value || n > max_value) {
            ec = make_error_code(std::errc::result_out_of_range);
            return 0;
        }
    }

    return n;
}

template <typename IntT, typename CharIt
    , typename std::enable_if<std::is_integral<IntT>::value, int>::type = 0>
inline IntT
to_integer (CharIt first, CharIt last, IntT min_value, IntT max_value, std::error_code & ec)
{
    return to_integer<IntT, CharIt>(first, last, min_value, max_value, 10, ec);
}

template <typename IntT, typename CharIt
    , typename std::enable_if<std::is_integral<IntT>::value, int>::type = 0>
inline IntT
to_integer (CharIt first, CharIt last, IntT min_value, IntT max_value, int radix = 10)
{
    std::error_code ec;
    auto n = to_integer<IntT, CharIt>(first, last, min_value, max_value, radix, ec);

    if (ec)
        throw error{ec, "bad integer string representation"};

    return n;
}

} // pfs

