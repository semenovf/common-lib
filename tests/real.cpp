////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2024 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2024.10.15 Initial version.
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/fmt.hpp"
#include "pfs/real.hpp"
#include <cmath>
#include <iterator>
#include <limits>
#include <string>

template <typename RealT>
RealT std_string_to_real (char const * nptr, char ** endptr);

template <>
inline float std_string_to_real<float> (char const * nptr, char ** endptr)
{
    errno = 0;
    return std::strtof(nptr, endptr);
}

template <>
inline double std_string_to_real<double> (char const * nptr, char ** endptr)
{
    errno = 0;
    return strtod(nptr, endptr);
}


////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename RealT>
inline bool test_isinf (char const * s)
{
    auto result = pfs::to_real<RealT>(std::string{s}, '.');

    if (!result)
        return false;

    return std::isinf(*result);
}

template <typename RealT>
inline bool test_isnan (char const * s)
{
    auto result = pfs::to_real<RealT>(std::string{s}, '.');

    if (!result)
        return false;

    return std::isnan(*result);
}

template <typename RealT>
static bool test_string_to_real (char const * s, RealT sample_value, std::ptrdiff_t badpos_increment)
{
    bool success = true;
    RealT value;
    auto pos = s;
    /*auto success = */pfs::advance_real(pos, s + std::strlen(s), '.', value);

    CHECK_MESSAGE(value == sample_value, "1/3. Converted value equals to sample");

    if (value != sample_value)
        success = false;

    char * endptr = nullptr;
    RealT std_result = std_string_to_real<RealT>(s, & endptr);

    CHECK_MESSAGE(value == std_result, "2/3. Converted value equals to the result of std::strto[fd]()");

    if (value != std_result) {
        RealT absdiff = std::fabs(value - std_result);
        auto epsilon = std::numeric_limits<RealT>::epsilon();
        std::string comp_op = "<=";

        if (absdiff > epsilon)
            comp_op = ">";

        CHECK_MESSAGE(absdiff <= epsilon, fmt::format("Comparing with result of strto[fd]():\n"
            "\tabsdiff({:g}) = fabs(result({:g}) - sample({:g})\n"
            "\tabsdiff({:g}) {} epsilon({:g})\n"
            , absdiff, value, std_result, absdiff, comp_op, epsilon));

        success = (absdiff <= epsilon);
    }

    std::advance(pos, badpos_increment);

    CHECK_MESSAGE(pos == s + std::strlen(s), "3/3. Badpos match");

    if (pos != s + std::strlen(s))
        success = false;

    return success;
}

template <typename RealT>
void parse_test ()
{
    CHECK(test_isinf<RealT>("INFINITY"));
    CHECK(test_isinf<RealT>("-INFINITY"));
    CHECK(test_isinf<RealT>("+INFINITY"));
    CHECK(test_isinf<RealT>("+InFiNiTy"));
    CHECK(test_isinf<RealT>("-infinity"));
    CHECK(test_isinf<RealT>("+INFInity"));
    CHECK(test_isinf<RealT>("INF"));
    CHECK(test_isinf<RealT>("-INF"));
    CHECK(test_isinf<RealT>("+INF"));
    CHECK(test_isinf<RealT>("INFINITY$%^"));

    CHECK(test_isnan<RealT>("NAN"));
    CHECK(test_isnan<RealT>("-NAN"));
    CHECK(test_isnan<RealT>("+NAN"));
    CHECK(test_isnan<RealT>("nAN"));
    CHECK(test_isnan<RealT>("-nAn"));
    CHECK(test_isnan<RealT>("+nan"));
    CHECK(test_isnan<RealT>("NAN$%^"));

    CHECK(test_string_to_real<RealT>("-"  , RealT{0}, 1));
    CHECK(test_string_to_real<RealT>("+"  , RealT{0}, 1));
    CHECK(test_string_to_real<RealT>("   ", RealT{0}, 3));
    CHECK(test_string_to_real<RealT>(" ." , RealT{0}, 2));
    CHECK(test_string_to_real<RealT>(" . ", RealT{0}, 3));
    CHECK(test_string_to_real<RealT>(".e3", RealT{0}, 3));
    CHECK(test_string_to_real<RealT>("e3" , RealT{0}, 2));
    CHECK(test_string_to_real<RealT>(""   , RealT{0}, 0));

    CHECK(test_string_to_real<RealT>("0"  , RealT{0}, 0));
    CHECK(test_string_to_real<RealT>("00" , RealT{0}, 0));
    CHECK(test_string_to_real<RealT>("000", RealT{0}, 0));
    CHECK(test_string_to_real<RealT>(".1" , RealT{0.1}, 0));
    CHECK(test_string_to_real<RealT>(".12", RealT{0.12}, 0));
    CHECK(test_string_to_real<RealT>(".1234567890", RealT{0.1234567890}, 0));
    CHECK(test_string_to_real<RealT>("1.2e3"      , RealT{1.2e3}, 0));
    CHECK(test_string_to_real<RealT>(" 1.2e3"  , RealT{1.2e3}, 0));
    CHECK(test_string_to_real<RealT>(" +1.2e3" , RealT{1.2e3}, 0));
    CHECK(test_string_to_real<RealT>("+1.e3"   , RealT{1.e3}, 0));
    CHECK(test_string_to_real<RealT>("-1.2e3"  , RealT{-1.2e3}, 0));
    CHECK(test_string_to_real<RealT>("-1.2e3.5", RealT{-1.2e3}, 2));
    CHECK(test_string_to_real<RealT>(".1e" , RealT{0.1}, 1));
    CHECK(test_string_to_real<RealT>("-1.2e    ", RealT{-1.2}, 5));
    CHECK(test_string_to_real<RealT>("--1.2e3.5", RealT{0}, 9));
    CHECK(test_string_to_real<RealT>("a", RealT{0}, 1));
    CHECK(test_string_to_real<RealT>("-a", RealT{0}, 2));
    CHECK(test_string_to_real<RealT>(".1e3", RealT{.1e3}, 0));
    CHECK(test_string_to_real<RealT>(".1e-3", RealT{.1e-3}, 0));
    CHECK(test_string_to_real<RealT>(".1e-", RealT{.1}, 2));
    CHECK(test_string_to_real<RealT>(" .e", RealT{0}, 3));
    CHECK(test_string_to_real<RealT>(" .e-", RealT{0}, 4));
    CHECK(test_string_to_real<RealT>(" e"  , RealT{0}, 2));
    CHECK(test_string_to_real<RealT>(" e0" , RealT{0}, 3));
    CHECK(test_string_to_real<RealT>(" ee" , RealT{0}, 3));
    CHECK(test_string_to_real<RealT>(" -e" , RealT{0}, 3));
    CHECK(test_string_to_real<RealT>(" .9" , RealT{0.9}, 0));
    CHECK(test_string_to_real<RealT>(" 0.9", RealT{0.9}, 0));
    CHECK(test_string_to_real<RealT>(" ..9", RealT{0}, 4));
    CHECK(test_string_to_real<RealT>("9."  , RealT{9.0}, 0));
    CHECK(test_string_to_real<RealT>("9"   , RealT{9.0}, 0));
    CHECK(test_string_to_real<RealT>("9.0" , RealT{9.0}, 0));
    CHECK(test_string_to_real<RealT>("9.0000"    , RealT{9.0}, 0));
    CHECK(test_string_to_real<RealT>("9.00001"   , RealT{9.00001}, 0));
    CHECK(test_string_to_real<RealT>("009"       , RealT{9.0}, 0));
    CHECK(test_string_to_real<RealT>("0.09e02"   , RealT{9.0}, 0));
    // CHECK(test_string_to_real<RealT>("2.22e-32"  , RealT{2.22e-32}, 0)); // FIXME
    CHECK(test_string_to_real<RealT>("1.34"      , RealT{1.34}, 0));
    CHECK(test_string_to_real<RealT>("12.34"     , RealT{12.34}, 0));
    CHECK(test_string_to_real<RealT>("123.456"   , RealT{123.456}, 0));

    CHECK(test_string_to_real<RealT>("999999999999999999" , RealT{999999999999999999.0}, 0));
    CHECK(test_string_to_real<RealT>("000000000999999999" , RealT{000000000999999999.0}, 0));
    CHECK(test_string_to_real<RealT>("0.99999999999999999" , RealT{0.99999999999999999}, 0));
    // CHECK(test_string_to_real<RealT>("0.9999999999999999999999999999999999" , RealT{0.9999999999999999999999999999999999}, 0)); // FIXME

    CHECK(test_string_to_real<RealT>("1", RealT{1.0}, 0));
    CHECK(test_string_to_real<RealT>("12", RealT{12.0}, 0));
    CHECK(test_string_to_real<RealT>("123", RealT{123.0}, 0));
    CHECK(test_string_to_real<RealT>("1234", RealT{1234.0}, 0));
    CHECK(test_string_to_real<RealT>("12345", RealT{12345.0}, 0));
    CHECK(test_string_to_real<RealT>("123456", RealT{123456.0}, 0));
    CHECK(test_string_to_real<RealT>("1234567", RealT{1234567.0}, 0));
    CHECK(test_string_to_real<RealT>("12345678", RealT{12345678.0}, 0));
    CHECK(test_string_to_real<RealT>("123456789", RealT{123456789.0}, 0));
    CHECK(test_string_to_real<RealT>("1234567890", RealT{1234567890.0}, 0));
    CHECK(test_string_to_real<RealT>("12345678901", RealT{12345678901.0}, 0));
    CHECK(test_string_to_real<RealT>("123456789012", RealT{123456789012.0}, 0));
    CHECK(test_string_to_real<RealT>("1234567890123", RealT{1234567890123.0}, 0));
    CHECK(test_string_to_real<RealT>("12345678901234", RealT{12345678901234.0}, 0));
    CHECK(test_string_to_real<RealT>("123456789012345", RealT{123456789012345.0}, 0));
    CHECK(test_string_to_real<RealT>("1234567890123456", RealT{1234567890123456.0}, 0));
    CHECK(test_string_to_real<RealT>("12345678901234567", RealT{12345678901234567.0}, 0));
    CHECK(test_string_to_real<RealT>("123456789012345678", RealT{123456789012345678.0}, 0));
    CHECK(test_string_to_real<RealT>("1234567890123456789", RealT{1234567890123456789.0}, 0));
    CHECK(test_string_to_real<RealT>("12345678901234567890", RealT{12345678901234567890.0}, 0));
    CHECK(test_string_to_real<RealT>("123456789012345678901", RealT{123456789012345678901.0}, 0));
    // CHECK(test_string_to_real<RealT>("1234567890123456789012", RealT{1234567890123456789012.0}, 0)); // FIXME
    CHECK(test_string_to_real<RealT>("12345678901234567890123", RealT{12345678901234567890123.0}, 0));
    CHECK(test_string_to_real<RealT>("123456789012345678901234", RealT{123456789012345678901234.0}, 0));
    CHECK(test_string_to_real<RealT>("1234567890123456789012345", RealT{1234567890123456789012345.0}, 0));
    CHECK(test_string_to_real<RealT>("12345678901234567890123456", RealT{12345678901234567890123456.0}, 0));
    CHECK(test_string_to_real<RealT>("123456789012345678901234567", RealT{123456789012345678901234567.0}, 0));
    CHECK(test_string_to_real<RealT>("1234567890123456789012345678", RealT{1234567890123456789012345678.0}, 0));
    CHECK(test_string_to_real<RealT>("12345678901234567890123456789", RealT{12345678901234567890123456789.0}, 0));
}

TEST_CASE("parse") {
    parse_test<float>();
    parse_test<double>();
}
