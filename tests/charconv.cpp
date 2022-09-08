////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2021.10.13 Initial version.
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define ANKERL_NANOBENCH_IMPLEMENT
#include "doctest.h"
#include "pfs/bits/operationsystem.h"
#include "pfs/charconv.hpp"
#include <limits>

template <typename IntT>
void parse_integer_limits (std::string min_str, std::string max_str)
{
    IntT value = 0;

    auto res = pfs::parse_integer(min_str.begin(), min_str.end(), value, 10);
    CHECK_EQ(value, (std::numeric_limits<IntT>::min)());
    CHECK_EQ(res.ptr, min_str.end());
    CHECK_EQ(res.ec, std::errc{});

    res = pfs::parse_integer(max_str.begin(), max_str.end(), value, 10);
    CHECK_EQ(value, (std::numeric_limits<IntT>::max)());
    CHECK_EQ(res.ptr, max_str.end());
    CHECK_EQ(res.ec, std::errc{});
}

TEST_CASE("parse integer limits") {
    parse_integer_limits<std::int8_t>("-128", "127");
    parse_integer_limits<std::uint8_t>("0", "255");
    parse_integer_limits<std::int16_t>("-32768", "32767");
    parse_integer_limits<std::uint16_t>("0", "65535");
    parse_integer_limits<std::int32_t>("-2147483648", "2147483647");
    parse_integer_limits<std::uint32_t>("0", "4294967295");

#ifdef PFS_OS_64BITS
    parse_integer_limits<std::int64_t>("-9223372036854775808", "9223372036854775807");
    parse_integer_limits<std::uint64_t>("0", "18446744073709551615");
#endif
}

template <typename IntT>
void parse_integer_over (std::string over_str, int endptr_index)
{
    IntT value = 42;
    auto res = pfs::parse_integer(over_str.begin(), over_str.end(), value, 10);

    // Unmodified
    CHECK_EQ(value, 42);

    // Stop on overflowed position
    auto endptr = over_str.begin();
    std::advance(endptr, endptr_index);
    CHECK_EQ(res.ptr, endptr);

    // Out of range error
    CHECK_EQ(res.ec, std::errc{std::errc::result_out_of_range});
}

TEST_CASE("parse overflow") {
    parse_integer_over<std::int8_t>("128", 2);
    parse_integer_over<std::uint8_t>("256", 2);

    parse_integer_over<std::int16_t>("32768", 4);
    parse_integer_over<std::uint16_t>("65536", 4);
    parse_integer_over<std::int32_t>("2147483648", 9);
    parse_integer_over<std::uint32_t>("4294967296", 9);

#ifdef PFS_OS_64BITS
    parse_integer_over<std::int64_t>("9223372036854775808", 18);
    parse_integer_over<std::uint64_t>("18446744073709551616", 19);
#endif
}

template <typename IntT>
void parse_integer_under (std::string under_str, int endptr_index)
{
    IntT value = 42;
    auto res = pfs::parse_integer(under_str.begin(), under_str.end(), value, 10);

    // Unmodified
    CHECK_EQ(value, 42);

    // Stop on underflowed position
    auto endptr = under_str.begin();
    std::advance(endptr, endptr_index);
    CHECK_EQ(res.ptr, endptr);

    // Out of range error
    CHECK_EQ(res.ec, std::errc{std::errc::result_out_of_range});
}

TEST_CASE("parse underflow") {
    parse_integer_under<std::int8_t>("-129", 3);
    parse_integer_under<std::int16_t>("-32769", 5);
    parse_integer_under<std::int32_t>("-2147483649", 10);

#ifdef PFS_OS_64BITS
    parse_integer_under<std::int64_t>("-9223372036854775809", 19);
#endif
}

TEST_CASE("parse unsigned integer from signed") {
    std::uint8_t value = 42;
    std::string signed_str = "-1";

    auto res = pfs::parse_integer(signed_str.begin(), signed_str.end(), value, 10);

    // Unmodified
    CHECK_EQ(value, 42);

    // Stop on underflowed position
    CHECK_EQ(res.ptr, signed_str.begin());

    CHECK_EQ(res.ec, std::errc{std::errc::invalid_argument});
}
