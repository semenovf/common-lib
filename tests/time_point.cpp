////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2021.11.20 Initial version.
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/time_point.hpp"

TEST_CASE("utc_offset") {
    {
        auto utc_offset = pfs::utc_offset();
        fmt::print("UTC offset: {:+} seconds\n", utc_offset);
        fmt::print("UTC offset formatted: {}\n", pfs::stringify_utc_offset(utc_offset));
    }

    {
        std:: time_t utc_offset = -1 * (30 * 60 + 12 * 3600);
        fmt::print("UTC offset: {:+} seconds\n", utc_offset);
        fmt::print("UTC offset formatted: {}\n", pfs::stringify_utc_offset(utc_offset));

        CHECK(pfs::stringify_utc_offset(utc_offset) == "-1230");
    }
}

TEST_CASE("make/date_time") {
    auto u1 = pfs::utc_time_point::make(1970, 1, 1, 0, 0, 0, 0);
    auto dt1 = u1.to_date_time();

    CHECK(dt1.year  == 1970);
    CHECK(dt1.month == 1);
    CHECK(dt1.day   == 1);
    CHECK(dt1.hours == 0);
    CHECK(dt1.minutes == 0);
    CHECK(dt1.seconds == 0);
    CHECK(dt1.millis == 0);

    auto u2 = pfs::utc_time_point::make(1969, 12, 31, 23, 59, 59, 234);
    auto dt2 = u2.to_date_time();

    CHECK(dt2.year    == 1969);
    CHECK(dt2.month   == 12);
    CHECK(dt2.day     == 31);
    CHECK(dt2.hours   == 23);
    CHECK(dt2.minutes == 59);
    CHECK(dt2.seconds == 59);
    CHECK(dt2.millis  == 234);

    auto u3 = pfs::utc_time_point::make(1970, 1, 1, 0, 0, 0, 234);
    auto dt3 = u3.to_date_time();

    CHECK(dt3.year    == 1970);
    CHECK(dt3.month   == 1);
    CHECK(dt3.day     == 1);
    CHECK(dt3.hours   == 0);
    CHECK(dt3.minutes == 0);
    CHECK(dt3.seconds == 0);
    CHECK(dt3.millis  == 234);

    auto u4 = pfs::utc_time_point::make(2022, 11, 28, 13, 52, 34, 234, 6, 0);
    auto dt4 = u4.to_date_time();
    CHECK(dt4.year  == 2022);
    CHECK(dt4.month == 11);
    CHECK(dt4.day   == 28);
    CHECK(dt4.hours == 7);
    CHECK(dt4.minutes == 52);
    CHECK(dt4.seconds == 34);
    CHECK(dt4.millis == 234);

    auto u5 = pfs::utc_time_point::make(2022, 11, 28, 13, 52, 34, 234, -4, 0);
    auto dt5 = u5.to_date_time();
    CHECK(dt5.year  == 2022);
    CHECK(dt5.month == 11);
    CHECK(dt5.day   == 28);
    CHECK(dt5.hours == 17);
    CHECK(dt5.minutes == 52);
    CHECK(dt5.seconds == 34);
    CHECK(dt5.millis == 234);

    ////////////////////////////////////////////////////////////////////////////
    // local time point
    ////////////////////////////////////////////////////////////////////////////
    for (auto x: { & u1, & u2, & u3, & u4, & u5} ) {
        auto loc = pfs::local_time_point_cast(*x);
        auto utc = operator + (*x, std::chrono::seconds(pfs::utc_offset()));
        auto ldt = loc.to_date_time();
        auto udt = utc.to_date_time();

        CHECK(ldt.year    == udt.year);
        CHECK(ldt.month   == udt.month);
        CHECK(ldt.day     == udt.day);
        CHECK(ldt.hours   == udt.hours);
        CHECK(ldt.minutes == udt.minutes);
        CHECK(ldt.seconds == udt.seconds);
        CHECK(ldt.millis  == udt.millis);
    }

    {
        auto u1 = pfs::utc_time_point::make(2022, 11, 28, 13, 52, 34, 234);
        auto l1 = pfs::local_time_point::make(2022, 11, 28, 13, 52, 34, 234);
        auto u2 = pfs::utc_time_point_cast(l1);
        fmt::print("-- UTC time            : {} {}\n", u1.to_iso8601(), u1.to_millis());
        fmt::print("-- Local time          : {} {}\n", l1.to_iso8601(), l1.to_millis());
        fmt::print("-- UTC time (converted): {} {}\n", u2.to_iso8601(), u2.to_millis());
    }
}

TEST_CASE("from_iso8601") {
    std::error_code ec;

    auto u1 = pfs::utc_time_point::from_iso8601("1970-01-01T00:00:00.000+0000", ec);
    REQUIRE_FALSE(ec);
    CHECK(u1.to_iso8601() == "1970-01-01T00:00:00.000+0000");

    auto u2 = pfs::utc_time_point::from_iso8601("1969-12-31T23:59:59.234+0000", ec);
    REQUIRE_FALSE(ec);
    CHECK(u2.to_iso8601() == "1969-12-31T23:59:59.234+0000");

    auto u3 = pfs::utc_time_point::from_iso8601("1970-01-01T00:00:00.234+0000", ec);
    REQUIRE_FALSE(ec);
    CHECK(u3.to_iso8601() == "1970-01-01T00:00:00.234+0000");

    auto u4 = pfs::utc_time_point::from_iso8601("2022-11-28T13:52:34.234+0600", ec);
    REQUIRE_FALSE(ec);
    CHECK(u4.to_iso8601() == "2022-11-28T07:52:34.234+0000");

    auto u5 = pfs::utc_time_point::from_iso8601("2022-11-28T13:52:34.234-0400", ec);
    REQUIRE_FALSE(ec);
    CHECK(u5.to_iso8601() == "2022-11-28T17:52:34.234+0000");

    char const * BADS[] = {
          "202y-11-22T11:33:42.999+0500" // bad year
        , "2021-13-22T11:33:42.999+0500" // bad month
        , "2021-11-32T11:33:42.999+0500" // bad day
        , "2021-11-22T25:33:42.999+0500" // bad hour
        , "2021-11-22T11:60:42.999+0500" // bad min
        , "2021-11-22T11:33:62.999+0500" // bad sec
        , "2021-11-22T11:33:42.9w9+0500" // bad millis
        , "2021-11-22T11:33:42.999+2401" // bad timezone offset
        , "2021-11-22 11:33:42.999+2401" // bad delimiter
    };

    for (auto const & s: BADS) {
        {
            std::error_code ec;
            auto bad = pfs::utc_time_point::from_iso8601(s, ec);
            REQUIRE(bad == pfs::utc_time_point{});
            REQUIRE(ec == std::make_error_code(std::errc::invalid_argument));

            CHECK_THROWS_AS(pfs::utc_time_point::from_iso8601(s), pfs::error);
        }
    }
}

// Logic, arithmetic operations
TEST_CASE("utc_time_point") {
    {
        // Zero seconds since epoch
        pfs::utc_time_point u;
        pfs::utc_time_point u1 {u};
        pfs::utc_time_point u2 {std::chrono::milliseconds{0}};
        pfs::utc_time_point u3 {std::chrono::seconds{0}};

        CHECK(u == u1);
        CHECK(u == u2);
        CHECK(u == u3);

        // One second since epoch
        pfs::utc_time_point u4 {std::chrono::seconds{1}};

        CHECK(u != u4);
        CHECK(u < u4);
        CHECK(u <= u4);
        CHECK(u4 > u);
        CHECK(u4 >= u);

        auto diff1 = u4 - u;
        auto diff2 = u - u4;

        CHECK(diff1 == std::chrono::seconds{1});
        CHECK(diff2 == -std::chrono::seconds{1});

        u -= std::chrono::hours{24};
        CHECK(u.to_iso8601() == "1969-12-31T00:00:00.000+0000");

        u += std::chrono::hours{48};
        CHECK(u.to_iso8601() == "1970-01-02T00:00:00.000+0000");
    }
}

TEST_CASE("cast") {
    {
        auto utc = pfs::current_utc_time_point();
        auto loc = pfs::current_local_time_point();
        fmt::print("Current UTC time  : {}\n", utc.to_iso8601());
        fmt::print("Current local time: {}\n", loc.to_iso8601());
    }

    {
        auto loc = pfs::current_local_time_point();
        auto utc = pfs::utc_time_point_cast(loc);

        CHECK_EQ(utc + std::chrono::seconds{pfs::utc_offset()}, loc);
        CHECK_EQ(loc - std::chrono::seconds{pfs::utc_offset()}, utc);
    }

    {
        auto utc = pfs::current_utc_time_point();
        auto loc = pfs::local_time_point_cast(utc);

        CHECK_EQ(utc + std::chrono::seconds{pfs::utc_offset()}, loc);
        CHECK_EQ(loc - std::chrono::seconds{pfs::utc_offset()}, utc);
    }
}
