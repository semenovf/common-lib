////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2020.03.28 Initial version (inspired from https://github.com/semenovf/pfs)
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "compare.hpp"
#include "numeric.hpp"
#include <limits>
#include <ratio>
#include <string>
#include <cmath>
#include <type_traits>
#include <stdexcept>

namespace pfs {

//
// Inspired from Boost::rational.
// See also http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3363.html
//

template <typename IntType>
class rational : public compare_operations
{
public:
    using int_type = IntType;

private:
    int_type _num;
    int_type _den; // always > 0

private:
    static const intmax_t * poweroften ()
    {
        static const intmax_t __n = 1000000000;

        static const intmax_t __poweroften[] = {
            1 , 10, 100, 1000, 10000, 100000, 1000000
            , 10000000, 100000000
            , 1000000000 // <= __n
            , __n * 10        // INT64_C(10000000000)
            , __n * 100       // INT64_C(100000000000)
            , __n * 1000      // INT64_C(1000000000000)
            , __n * 10000     // INT64_C(10000000000000)
            , __n * 100000    // INT64_C(100000000000000)
            , __n * 1000000   // INT64_C(10000000000000000)
            , __n * 10000000  // INT64_C(100000000000000000)
            , __n * 100000000 // INT64_C(1000000000000000000)
        };

        return __poweroften;
    }

    static std::uint8_t poweroften_size ()
    {
        static const std::uint8_t __poweroften_size = 18;
        return __poweroften_size;
    }

public:
    rational () : _num(0), _den(1) {}

    rational (int_type num, int_type den = 1)
        : _num(sign_of(num) * sign_of(den) * std::abs(num))
        , _den(std::abs(den))
    {
        if (den == 0)
            throw std::overflow_error("zero denominator");
    }

    template <intmax_t Num, intmax_t Denom>
    rational (std::ratio<Num, Denom> ratio)
    {
        if (!(Num <= std::numeric_limits<int_type>::max()
                && Num >= std::numeric_limits<int_type>::min()))
            throw std::overflow_error("numinator overflow");

        if (!(Denom <= std::numeric_limits<int_type>::max()
                && Denom >= std::numeric_limits<int_type>::min()))
            throw std::overflow_error("denominator overflow");

        _num = static_cast<decltype(_num)>(sign_of(ratio.num) * sign_of(ratio.den) * std::abs(ratio.num));
        _den = static_cast<decltype(_den)>(std::abs(ratio.den));
    }

    rational (rational const & rhs) = default;
    rational (rational && rhs) = default;
    rational & operator = (rational const & rhs) = default;
    rational & operator = (rational && rhs) = default;
    ~rational () = default;

    rational & assign (int_type n, int_type d)
    {
        *this = rational(n, d);
        return *this;
    }

    constexpr int_type numerator () const
    {
        return _num;
    }

    constexpr int_type denominator () const
    {
        return _den;
    }

    // Unary minus
    rational operator - () const noexcept
    {
        rational result(*this);
        result._num = - result._num;
        return result;
    }

    // Unary plus
    rational operator + () const noexcept
    {
        return *this;
    }

    rational abs () const noexcept
    {
        if (_num < 0)
            return -*this;
        return *this;
    }

    /**
     * @return An equivalent fraction with all common factors between the
     *         numerator and the denominator removed.
     */
    rational reduce () const noexcept
    {
        int_type gcd = pfs::gcd(_num, _den);
        return rational(_num / gcd, _den / gcd);
    }

    /**
     * @return Result of fraction invert operation.
     */
    rational invert () const
    {
        if (_num == int_type(0))
            throw std::overflow_error("zero denominator");

        return rational(sign_of(_num) * _den, std::abs(_num));
    }

    /**
     * @return @c true if the denominator is a power of ten, @c false otherwise.
     */
    bool is_decimal () const noexcept
    {
        static const intmax_t * __poweroften = poweroften();
        static const uint8_t    __poweroften_size = poweroften_size();

        for (int i = 0; i < __poweroften_size && _den >= __poweroften[i]; ++i) {
            if (_den == __poweroften[i])
                return true;
            if (_den % __poweroften[i])
                return false;
        }

        return false;
    }

    std::string to_string () const noexcept;
    std::string to_pretty_string (char decimal_point = '.') const noexcept;

    //
    // Compare
    //
    int compare (rational const & rhs) const;

    bool operator == (rational const & rhs) const
    {
        return compare(rhs) == 0;
    }

    bool operator < (rational const & rhs) const
    {
        return compare(rhs) < 0;
    }

    //
    // Arithmetic operators
    //
    rational & operator += (rational const & rhs);
    rational & operator -= (rational const & rhs);
    rational & operator *= (rational const & rhs);
    rational & operator /= (rational const & rhs);

    rational & operator += (int_type i) { _num += i * _den; return *this; }
    rational & operator -= (int_type i) { _num -= i * _den; return *this; }
    rational & operator *= (int_type i);
    rational & operator /= (int_type i);
};


template <typename IntType>
rational<IntType> & rational<IntType>::operator += (rational const & rhs)
{
    // This calculation avoids overflow, and minimises the number of expensive
    // calculations. Thanks to Nickolay Mladenov for this algorithm.
    //
    // Proof:
    // We have to compute a/b + c/d, where gcd(a,b)=1 and gcd(b,c)=1.
    // Let g = gcd(b,d), and b = b1*g, d=d1*g. Then gcd(b1,d1)=1
    //
    // The result is (a*d1 + c*b1) / (b1*d1*g).
    // Now we have to normalize this ratio.
    // Let's assume h | gcd((a*d1 + c*b1), (b1*d1*g)), and h > 1
    // If h | b1 then gcd(h,d1)=1 and hence h|(a*d1+c*b1) => h|a.
    // But since gcd(a,b1)=1 we have h=1.
    // Similarly h|d1 leads to h=1.
    // So we have that h | gcd((a*d1 + c*b1) , (b1*d1*g)) => h|g
    // Finally we have gcd((a*d1 + c*b1), (b1*d1*g)) = gcd((a*d1 + c*b1), g)
    // Which proves that instead of normalizing the result, it is better to
    // divide num and den by gcd((a*d1 + c*b1), g)

    // Protect against self-modification
    //int_type r_num = r._num;
    //int_type r_den = r._den;

    int_type r_num = rhs._num;
    int_type r_den = rhs._den;

    int_type g = pfs::gcd(_den, r_den);
    _den /= g;  // = b1 from the calculations above
    _num = _num * (r_den / g) + r_num * _den;
    g = pfs::gcd(_num, g);
    _num /= g;
    _den *= r_den/g;

    return *this;
}

template <typename IntType>
rational<IntType> & rational<IntType>::operator -= (rational const & rhs)
{
    // Protect against self-modification
    int_type r_num = rhs._num;
    int_type r_den = rhs._den;

    // This calculation avoids overflow, and minimises the number of expensive
    // calculations. It corresponds exactly to the += case above
    int_type g = pfs::gcd(_den, r_den);
    _den /= g;
    _num = _num * (r_den / g) - r_num * _den;
    g = pfs::gcd(_num, g);
    _num /= g;
    _den *= r_den/g;

    return *this;
}

template <typename IntType>
rational<IntType> & rational<IntType>::operator *= (rational const & rhs)
{
    // Protect against self-modification
    int_type r_num = rhs._num;
    int_type r_den = rhs._den;

    // Avoid overflow and preserve normalization
    int_type gcd1 = pfs::gcd(_num, r_den);
    int_type gcd2 = pfs::gcd(r_num, _den);
    _num = (_num/gcd1) * (r_num/gcd2);
    _den = (_den/gcd2) * (r_den/gcd1);
    return *this;
}

template <typename IntType>
rational<IntType> & rational<IntType>::operator /= (rational const & rhs)
{
    // Protect against self-modification
    int_type r_num = rhs._num;
    int_type r_den = rhs._den;

    // Avoid repeated construction
    int_type zero(0);

    if (r_num == zero)
        throw std::overflow_error("divide by zero");

    if (_num == zero)
        return *this;

    // Avoid overflow and preserve normalization
    int_type gcd1 = pfs::gcd(_num, r_num);
    int_type gcd2 = pfs::gcd(r_den, _den);
    _num = (_num/gcd1) * (r_den/gcd2);
    _den = (_den/gcd2) * (r_num/gcd1);

    if (_den < zero) {
        _num = -_num;
        _den = -_den;
    }
    return *this;
}

template <typename IntType>
inline rational<IntType> & rational<IntType>::operator *= (IntType i)
{
    // Avoid overflow and preserve normalization
    int_type gcd = pfs::gcd(i, _den);
    _num *= i / gcd;
    _den /= gcd;

    return *this;
}

template <typename IntType>
rational<IntType> & rational<IntType>::operator /= (IntType i)
{
    // Avoid repeated construction
    int_type const zero(0);

    if (i == zero)
        throw std::overflow_error("divide by zero");

    if (_num == zero) return *this;

    // Avoid overflow and preserve normalization
    int_type const gcd = pfs::gcd(_num, i);
    _num /= gcd;
    _den *= i / gcd;

    if (_den < zero) {
        _num = -_num;
        _den = -_den;
    }

    return *this;
}

namespace details {

    typedef uint64_t multiply_param_type;
    typedef uint32_t multiply_part_type;
    inline constexpr uint32_t uint_lo(uint64_t x) { return static_cast<uint32_t>(x & 0xffffffff); }
    inline constexpr uint32_t uint_hi(uint64_t x) { return static_cast<uint32_t>(x >> 32); }
    static uint64_t const uint_carry = (static_cast<uint64_t>(1) << 32);

inline void multiply (multiply_param_type a, multiply_param_type b
        , multiply_param_type & hi, multiply_param_type & lo)
{
    multiply_part_type al = uint_lo(a);
    multiply_part_type ah = uint_hi(a);
    multiply_part_type bl = uint_lo(b);
    multiply_part_type bh = uint_hi(b);

    multiply_param_type r0 = static_cast<multiply_param_type>(al) * bl;
    multiply_param_type r1 = static_cast<multiply_param_type>(al) * bh;
    multiply_param_type r2 = static_cast<multiply_param_type>(ah) * bl;
    multiply_param_type r3 = static_cast<multiply_param_type>(ah) * bh;

    r1 += uint_hi(r0);
    r1 += r2;

    if (r1 < r2)
        r3 += uint_carry;

    hi = r3 + uint_hi(r1);
    lo = (static_cast<uint64_t>(uint_lo(r1)) << 32) + uint_lo(r0);
}

} // namespace details

template <typename IntType>
int rational<IntType>::compare (rational const & rhs) const
{
    if (_den == rhs._den) {
        return _num < rhs._num
            ? -1
            : _num > rhs._num
                ? 1 : 0;
    }

    int sign1 = sign_of(*this);
    int sign2 = sign_of(rhs);

    if (sign1 < sign2)
        return -1;

    if (sign2 < sign1)
        return 1;

    //
    // Bellow compared rationals has same signs
    //

    // Quotients
    int_type q1 = static_cast<int_type>(_num / _den);
    int_type q2 = static_cast<int_type>(rhs._num / rhs._den);

    // [6]
    if (q1 != q2)
        return q1 < q2 ? -1 : 1;

    if (sizeof(intmax_t) > sizeof(int_type)) {
        intmax_t m1 = static_cast<intmax_t>(_num) * rhs._den;
        intmax_t m2 = static_cast<intmax_t>(rhs._num) * _den;

        return m1 == m2
                ? 0
                : m1 < m2 ? -1 : 1;
    }

    //
    // Emulate comparison of big integers
    //
    uint64_t a1 = std::abs(_num);
    uint64_t b1 = rhs._den;
    uint64_t a2 = std::abs(rhs._num);
    uint64_t b2 = _den;

    uint64_t lo1, hi1, lo2, hi2;

    details::multiply(a1, b1, lo1, hi1);
    details::multiply(a2, b2, lo2, hi2);

    if (hi1 == hi2)
        return lo1 == lo2
                ? 0
                : lo1 < lo2
                    ? -1 * sign1
                    : sign1;

    if (hi1 < hi2)
        return -1 * sign1;

    return sign1;
}

template <typename IntType>
std::string rational<IntType>::to_string () const noexcept
{
    using std::to_string;
    std::string result;

    result += to_string(_num);
    result += '/';
    result += to_string(_den);

    return result;
}

template <typename IntType>
std::string rational<IntType>::to_pretty_string (char decimal_point) const noexcept
{
    using std::to_string;
    std::string result;

    if (_num % _den == 0) {
        result += to_string(_num / _den);
    } else if (std::abs(_num) == _den) {
        result += to_string(sign_of(_num));
    } else if (is_decimal()) {
        result += to_string(_num / _den);
        result += decimal_point;

        int_type x = _den / _num;

        while ((x /= 10) > 0)
            result += '0';

        result += to_string((_num > 0 ? _num : -_num) % _den);
    } else {
        result += to_string(_num);
        result += '/';
        result += to_string(_den);
    }

    return result;
}

template <typename IntType>
inline rational<IntType> abs (rational<IntType> const & x)
{
    return x.abs();
}

template <typename IntType>
inline int sign_of (rational<IntType> const & x)
{
    return sign_of(x.numerator());
}

template <typename IntType>
inline std::string to_string (rational<IntType> const & r)
{
    return r.to_string();
}

template <typename IntType>
inline std::string to_pretty_string (rational<IntType> const & r
        , std::string const & decimal_point = ".")
{
    return r.to_pretty_string(decimal_point);
}


template <typename IntType>
inline typename rational<IntType>::int_type floor (rational<IntType> const & val)
{
    typename rational<IntType>::int_type n = val.numerator();
    typename rational<IntType>::int_type d = val.denominator();

//  Algorithm:
//     if (n % d == 0) return n / d;
//     if (n > 0) return n / d;
//     if (n < 0) return (n / d) - 1;
    typename rational<IntType>::int_type result = n / d;

    if ((n < 0) && (n % d != 0)) --result;

    return result;
}

template <typename IntType>
inline typename rational<IntType>::int_type ceil (rational<IntType> const & val)
{
    IntType n = val.numerator();
    IntType d = val.denominator();

//  Algorithm:
//     if (n % d == 0) return n / d;
//     if (n > 0) return (n / d) + 1;
//     if (n < 0) return n / d;
    typename rational<IntType>::int_type result = n / d;

    if ((n > 0) && (n % d != 0)) ++result;

    return result;
}

//rational::int_type trunc(const rational& val);

template <typename IntType>
inline rational<IntType> operator + (rational<IntType> const & lhs, rational<IntType> const & rhs)
{
    rational<IntType> result(lhs);
    return result += rhs;
}

template <typename IntType>
inline rational<IntType> operator - (rational<IntType> const & lhs, rational<IntType> const & rhs)
{
    rational<IntType> result(lhs);
    return result -= rhs;
}

template <typename IntType>
inline rational<IntType> operator * (rational<IntType> const & lhs, rational<IntType> const & rhs)
{
    rational<IntType> result(lhs);
    return result *= rhs;
}

template <typename IntType>
inline rational<IntType> operator / (rational<IntType> const & lhs, rational<IntType> const & rhs)
{
    rational<IntType> result(lhs);
    return result /= rhs;
}

template <typename IntType>
inline rational<IntType> operator + (rational<IntType> const & lhs, typename rational<IntType>::int_type rhs)
{
    rational<IntType> result(lhs);
    return result += rhs;
}

template <typename IntType>
inline rational<IntType> operator - (rational<IntType> const & lhs, typename rational<IntType>::int_type rhs)
{
    rational<IntType> result(lhs);
    return result -= rhs;
}

template <typename IntType>
inline rational<IntType> operator * (rational<IntType> const & lhs, typename rational<IntType>::int_type rhs)
{
    rational<IntType> result(lhs);
    return result *= rhs;
}

template <typename IntType>
inline rational<IntType> operator / (rational<IntType> const & lhs, typename rational<IntType>::int_type rhs)
{
    rational<IntType> result(lhs);
    return result /= rhs;
}

template <typename IntType>
inline rational<IntType> operator + (typename rational<IntType>::int_type lhs, rational<IntType> const & rhs)
{
    return rational<IntType>(lhs) + rhs;
}

template <typename IntType>
inline rational<IntType> operator - (typename rational<IntType>::int_type lhs, rational<IntType> const & rhs)
{
    return rational<IntType>(lhs) - rhs;
}

template <typename IntType>
inline rational<IntType> operator * (typename rational<IntType>::int_type lhs, rational<IntType> const & rhs)
{
    return rational<IntType>(lhs) * rhs;
}

template <typename IntType>
inline rational<IntType> operator / (typename rational<IntType>::int_type lhs, rational<IntType> const & rhs)
{
    return rational<IntType>(lhs) / rhs;
}

// template <typename> struct is_rational : false_type {};
// template <> struct is_rational<rational<signed char> > : public true_type {};
// template <> struct is_rational<rational<unsigned char> > : public true_type {};
// template <> struct is_rational<rational<short int> > : public true_type {};
// template <> struct is_rational<rational<unsigned short int> > : public true_type {};
// template <> struct is_rational<rational<int> > : public true_type {};
// template <> struct is_rational<rational<unsigned int> > : public true_type {};
// template <> struct is_rational<rational<long int> > : public true_type {};
// template <> struct is_rational<rational<unsigned long int> > : public true_type {};
//
// #if PFS_HAVE_LONG_LONG
// template <> struct is_rational<rational<long long int> > : public true_type {};
// template <> struct is_rational<rational<unsigned long long int> > : public true_type {};
// #endif

} // namespace pfs

//#include <pfs/lexical_cast/strtorational.hpp>

// namespace pfs {
//
// /**
//  * @brief Converts string representation of rational number.
//  * @details String representation of rational number must match one
//  *          of the following formats:
//  *          <ul>
//  *          <li><decimal_point><decimal_digit>+
//  *          <li><decimal_digit>+ [ <decimal_point> [ <decimal_digit>+ ]
//  *          <li><decimal_digit>+ '/' <decimal_digit>++
//  *          </ul>
//  *
//  *          Valid examples:
//  *          <ul>
//  *          <li>.14159
//  *          <li>3
//  *          <li>3.
//  *          <li>3.14159
//  *          <li>1/2
//  *          </ul>
//  *
//  * @param s String representation of rational number.
//  * @param ec Resulting error code if lexical casting was fail.
//  * @param decimal_point Decimal point representation.
//  */
// template <typename RationalT>
// inline typename pfs::enable_if<pfs::is_rational<RationalT>::value, RationalT>::type
// lexical_cast (string const & s, error_code & ec, string::value_type decimal_point = '.')
// {
// //     if (s.empty()) {
// //         ec = pfs::make_error_code(lexical_cast_errc::invalid_string);
// //         return RationalT();
// //     }
// //
// //     typedef string::const_iterator iterator;
// //     iterator badpos;
// //
// //     RationalT result = string_to_rational<RationalT, iterator>(s.cbegin()
// //             , s.cend()
// //             , & badpos);
// //
// //     if (badpos != s.cend())
// //         ec = pfs::make_error_code(lexical_cast_errc::invalid_string);
// //
// //     return result;
//     return RationalT();
// }
//
// template <typename RationalT>
// inline typename pfs::enable_if<pfs::is_rational<RationalT>::value, RationalT>::type
// lexical_cast (string const & s, string::value_type decimal_point = '.')
// {
//     error_code ec;
//     RationalT result = lexical_cast<RationalT>(s, ec, decimal_point);
//
//     if (ec) PFS_THROW(bad_lexical_cast(ec));
//
//     return result;
// }
//
//} // namespace pfs
