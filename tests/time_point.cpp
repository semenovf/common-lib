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

TEST_CASE("local_time_point") {
    {
        // Zero seconds since epoch
        pfs::local_time_point l;
        pfs::local_time_point l1 {l};
        pfs::local_time_point l2 {std::chrono::milliseconds{0}};
        pfs::local_time_point l3 {std::chrono::seconds{0}};

        CHECK(l == l1);
        CHECK(l == l2);
        CHECK(l == l3);

        // One second since epoch
        pfs::local_time_point l4 {std::chrono::seconds{1}};

        CHECK(l != l4);
        CHECK(l < l4);
        CHECK(l <= l4);
        CHECK(l4 > l);
        CHECK(l4 >= l);

        l -= std::chrono::hours{24};
        fmt::print("Local time of yesterday since epoch: {}\n", l.to_iso8601());

        auto now  = pfs::current_local_time_point();
        auto dur  = now.time_since_epoch();
        pfs::local_time_point now1 {dur};
        auto dur1 = now1.time_since_epoch();

        CHECK(now == now1);
        CHECK(dur == dur1);

        fmt::print("Local time: {}\n", now.to_iso8601());
    }
}

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

        u -= std::chrono::hours{24};
        fmt::print("UTC time of yesterday since epoch: {}\n", u.to_iso8601());

        auto now  = pfs::current_utc_time_point();
        auto dur  = now.time_since_epoch();
        pfs::local_time_point now1 {dur};
        auto dur1 = now1.time_since_epoch();

        CHECK(now == now1);
        CHECK(dur == dur1);

        fmt::print("UTC time: {}\n", now.to_iso8601());
    }

    {
        auto local = pfs::current_local_time_point();
        auto utc = pfs::utc_time_point_cast(local);
        fmt::print("UTC time (converted from local): {}\n", utc.to_iso8601());
    }

    {
        auto utc = pfs::current_utc_time_point();
        auto local = pfs::local_time_point_cast(utc);
        fmt::print("Local time (converted from UTC): {}\n", local.to_iso8601());
    }
}

TEST_CASE("make") {
    std::error_code ec;

    auto u1 = pfs::utc_time_point::make(1970,  1,  1,  0,  0,  0,   0,  0, 0, ec);
    REQUIRE_FALSE(ec);
    CHECK(u1.to_iso8601() == "1970-01-01T00:00:00.000+0000");

    auto u2 = pfs::utc_time_point::make(2022, 11, 28, 13, 52, 34, 234, 6, 0, ec);
    REQUIRE_FALSE(ec);
    CHECK(u2.to_iso8601() == "2022-11-28T07:52:34.234+0000");

    auto u3 = pfs::utc_time_point::make(2022, 11, 28, 13, 52, 34, 234, -4, 0, ec);
    REQUIRE_FALSE(ec);
    CHECK(u3.to_iso8601() == "2022-11-28T17:52:34.234+0000");

    auto l1 = pfs::local_time_point::make(1970, 1, 1, 0, 0, 0, 0, 0, 0, ec);
    REQUIRE_FALSE(ec);
    CHECK(l1 == pfs::local_time_point{});

    pfs::local_time_point::make(2022, 11, 28, 13, 52, 34, 234, 6, 0, ec);
    REQUIRE_FALSE(ec);

    pfs::local_time_point::make(2022, 11, 28, 13, 52, 34, 234, -4, 0, ec);
    REQUIRE_FALSE(ec);
}

TEST_CASE("from_iso8601") {
    std::error_code ec;

    auto ugood = pfs::utc_time_point::from_iso8601("2021-11-22T11:33:42.999-0600", ec);
    REQUIRE(ec == std::error_code{});
    CHECK(ugood.to_iso8601() == "2021-11-22T05:33:42.999+0000");

    auto lgood = pfs::local_time_point::from_iso8601("2021-11-22T11:33:42.999-0600", ec);
    REQUIRE(ec == std::error_code{});

    fmt::print("Local time for '2021-11-22T11:33:42.999-0600': {}\n", lgood.to_iso8601());

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

        {
            std::error_code ec;
            auto bad = pfs::local_time_point::from_iso8601(s, ec);
            REQUIRE(bad == pfs::local_time_point{});
            REQUIRE(ec == std::make_error_code(std::errc::invalid_argument));

            CHECK_THROWS_AS(pfs::local_time_point::from_iso8601(s), pfs::error);
        }
    }
}
