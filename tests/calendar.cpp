////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2020.06.20 Initial version
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/calendar.hpp"
#include "sstream"

namespace calendar = pfs::calendar;
using julian_day = calendar::julian_day;

TEST_CASE("comapre") {
    CHECK(julian_day{2019, 11, 27} == julian_day{2019, 11, 27});
    CHECK(julian_day{2019, 11, 27} != julian_day{2019, 11, 26});
    CHECK(julian_day{2019, 11, 27} <  julian_day{2019, 11, 28});
    CHECK(julian_day{2019, 11, 27} <= julian_day{2019, 11, 27});
    CHECK(julian_day{2019, 11, 27} <= julian_day{2019, 11, 28});
    CHECK(julian_day{2019, 11, 27} >  julian_day{2019, 11, 26});
    CHECK(julian_day{2019, 11, 27} >= julian_day{2019, 11, 27});
    CHECK(julian_day{2019, 11, 27} >= julian_day{2019, 11, 26});
}

TEST_CASE("leap year") {
    CHECK_FALSE(calendar::is_leap_year(2015));
    CHECK(calendar::is_leap_year(2016));
    CHECK_FALSE(calendar::is_leap_year(2017));
    CHECK_FALSE(calendar::is_leap_year(2018));
    CHECK_FALSE(calendar::is_leap_year(2019));
    CHECK(calendar::is_leap_year(2020));
}

TEST_CASE("min/max")
{
    CHECK_EQ(julian_day::min_day().native(), julian_day::raw_min_day);
    CHECK_EQ(julian_day::min_day(), julian_day::make((std::numeric_limits<int>::min)(), 1, 1));

    CHECK_EQ(julian_day::max_day().native(), julian_day::raw_max_day);
    CHECK_EQ(julian_day::max_day(), julian_day::make((std::numeric_limits<int>::max)(), 12, 31));

    CHECK_EQ(julian_day::epoch_day(), julian_day{1970, 1, 1});
}

TEST_CASE("julian day")
{
    CHECK_THROWS_AS(julian_day(0, 0, 0), pfs::error);

    CHECK_EQ(julian_day::make(1, 1, 1).native(), 1721424);

    // Changes from Julian to Gregorian between October 4/15, 1582.
    CHECK_EQ(julian_day::make(1582, 10, 3).native(), 2299159);
    CHECK_EQ(julian_day::make(1582, 10, 4).native(), 2299160);

    CHECK_THROWS_AS(julian_day::make(1582, 10, 5), pfs::error);
    CHECK_THROWS_AS(julian_day::make(1582, 10, 6), pfs::error); // invalid
    CHECK_THROWS_AS(julian_day::make(1582, 10, 7), pfs::error); // invalid
    CHECK_THROWS_AS(julian_day::make(1582, 10, 8), pfs::error); // invalid
    CHECK_THROWS_AS(julian_day::make(1582, 10, 9), pfs::error); // invalid
    CHECK_THROWS_AS(julian_day::make(1582, 10, 10), pfs::error); // invalid
    CHECK_THROWS_AS(julian_day::make(1582, 10, 11), pfs::error); // invalid
    CHECK_THROWS_AS(julian_day::make(1582, 10, 12), pfs::error); // invalid
    CHECK_THROWS_AS(julian_day::make(1582, 10, 13), pfs::error); // invalid
    CHECK_THROWS_AS(julian_day::make(1582, 10, 14), pfs::error); // invalid

    CHECK_EQ(julian_day::make(1582, 10, 15).native(), 2299161);
    CHECK_EQ(julian_day::make(1582, 10, 16).native(), 2299162);
    CHECK_EQ(julian_day::make(1600, 1, 1).native(), 2305448);

    CHECK_EQ(julian_day::make(2013, 9, 13).native(), 2456549);
    CHECK_EQ(julian_day::make(-2013, 9, 13).native(), 986431);

    CHECK_EQ(julian_day{2013, 9, 13}.days_to(julian_day{  2013,  9, 13}), 0);
    CHECK_EQ(julian_day{2013, 9, 13}.days_to(julian_day{  2013,  9, 12}), -1);
    CHECK_EQ(julian_day{2013, 9, 13}.days_to(julian_day{  2013,  6,  5}), -100);
    CHECK_EQ(julian_day{2013, 9, 13}.days_to(julian_day{  2010, 12, 18}), -1000);
    CHECK_EQ(julian_day{2013, 9, 13}.days_to(julian_day{  1986,  4, 28}), -10000);
    CHECK_EQ(julian_day{2013, 9, 13}.days_to(julian_day{  1739, 11, 29}), -100000);
    CHECK_EQ(julian_day{2013, 9, 13}.days_to(julian_day{  -726, 10, 25}), -1000000);
    CHECK_EQ(julian_day{2013, 9, 13}.days_to(julian_day{-25366,  2, 27}), -10000000);

    julian_day d1{2013, 9, 13};
    d1 = d1.add_days(-10000000);

    julian_day d2{-25366, 2, 27};
    CHECK_MESSAGE(d1 == d2, "13.9.2013 - 10000000 is a valid date and it is 27.2.25366 BC");

    CHECK_THROWS_AS_MESSAGE(julian_day(-25365, 2, 30), pfs::error, "30.2.25365 BC is invalid date");

    // 4.10.1582 + 1 day = 15.10.1582
    CHECK_EQ(julian_day{1582, 10, 4}.add_days(1), julian_day{1582, 10, 15});
    CHECK_EQ(julian_day{1582, 10, 15}.add_days(-1), julian_day{1582, 10, 4});

    CHECK(julian_day{2020, 2, 28}.native() == 2458908);
    CHECK(julian_day{2020, 2, 29}.native() == 2458909);
    CHECK(julian_day{2020, 3,  1}.native() == 2458910);
    CHECK(julian_day{2020, 6, 20}.native() == 2459021);

    auto di = julian_day::make(2458909).decompose();
    CHECK_EQ(di.year, 2020);
    CHECK_EQ(di.month, 2);
    CHECK_EQ(di.day_of_month, 29);
    CHECK_EQ(di.day_of_week, 6);
    CHECK_EQ(di.day_of_year, 31 + 29);
}

// From Qt's QDate class reference
TEST_CASE("valid")
{
    CHECK_MESSAGE(julian_day(2002, 5, 17).native(), "17.05.2002 is a valid date"); // true
    CHECK_THROWS_AS_MESSAGE(julian_day(2002, 2, 30).native(), pfs::error, "30.02.2002 is not a valid date: Feb 30 does not exist");
    CHECK_MESSAGE(julian_day(2004, 2, 29).native(), "29.02.2004 is a valid date: 2004 is a leap year");
    CHECK_MESSAGE(julian_day(2000, 2, 29).native(), "29.02.2004 is a valid date: 2000 is a leap year");
    CHECK_THROWS_AS_MESSAGE(julian_day(2006, 2, 29).native(), pfs::error, "29.02.2006 is not a valid date: 2006 is not a leap year");
    CHECK_THROWS_AS_MESSAGE(julian_day(2100, 2, 29).native(), pfs::error, "29.02.2100 is not a valid date: 2100 is not a leap year");
    CHECK_MESSAGE(julian_day(1202, 6,  6).native(), "06.06.1202 is a valid date: even though 1202 is pre-Gregorian");

    auto jday = julian_day::make(2456575);
    auto d = jday.decompose();
    auto year  = d.year;
    auto month = d.month;
    auto day   = d.day_of_month;

    std::stringstream ss;
    ss << "9.10.2013 == " << to_string(jday);
    CHECK_MESSAGE((julian_day(2013, 10, 9) == jday), ss.str().c_str());

    ss.clear();
    ss << to_string(jday) << " (9.10.2013) == " << day << '.' << month << '.' << year;
    CHECK_MESSAGE((julian_day(year, month, day) == jday), ss.str().c_str());
}

// From Qt's QDate class reference
TEST_CASE("periods")
{
    julian_day d1{1995, 5, 17}; // May 17, 1995
    julian_day d2{1995, 5, 20}; // May 20, 1995
    CHECK_MESSAGE((d1.days_to(d2) == 3), "Days bitween 17.05.1995 and 20.05.1995 is 3");
    CHECK_MESSAGE((d2.days_to(d1) == -3), "Days bitween 20.05.1995 and 17.05.1995 is -3");
}

TEST_CASE("stringify")
{
    using std::to_string;

    CHECK(to_string(julian_day{2020, 2, 28}, std::string("{:%Y-%m-%d}")) == std::string("2020-02-28"));
    CHECK(to_string(julian_day{2020, 2, 29}, std::string("{:%Y-%m-%d}")) == std::string("2020-02-29"));
    CHECK(to_string(julian_day{2020, 3,  1}, std::string("{:%Y-%m-%d}")) == std::string("2020-03-01"));
    CHECK(to_string(julian_day{2020, 6, 20}, std::string("{:%Y-%m-%d}")) == std::string("2020-06-20"));
    CHECK(to_string(julian_day{2013, 11, 28}, std::string("{:%Y-%m-%d}")) == std::string("2013-11-28"));

    // If the result is less than four digits it is left-padded with 0 to four digits.
    CHECK(to_string(julian_day{   1,  2,  3}, std::string("{:%Y-%m-%d}")) == std::string("0001-02-03"));
    // If the result is a single digit it is prefixed by 0.
    CHECK(to_string(julian_day{   1,  2,  3}, std::string("{:%y-%m-%d}")) == std::string("01-02-03"));

    CHECK(to_string(julian_day{2013, 11, 28}) == std::string("2013-11-28"));
    CHECK(to_string(julian_day{   1,  2,  3}) == std::string("0001-02-03"));
}

TEST_CASE("add months")
{
    CHECK_EQ(julian_day{2013, 01, 31}.add_months(1), julian_day{2013, 02, 28});
    CHECK_EQ(julian_day{2013, 01, 31}.add_months(-2), julian_day{2012, 11, 30});
}

TEST_CASE("add years")
{
    CHECK_EQ(julian_day{2013, 01, 31}.add_years(1), julian_day{2014, 01, 31});
    CHECK_EQ(julian_day{2013, 01, 31}.add_years(-2), julian_day{2011, 01, 31});
}

TEST_CASE("days in month")
{
    CHECK(calendar::days_in_month(2019,  1) == 31);
    CHECK(calendar::days_in_month(2019,  2) == 28);
    CHECK(calendar::days_in_month(2019,  3) == 31);
    CHECK(calendar::days_in_month(2019,  4) == 30);
    CHECK(calendar::days_in_month(2019,  5) == 31);
    CHECK(calendar::days_in_month(2019,  6) == 30);
    CHECK(calendar::days_in_month(2019,  7) == 31);
    CHECK(calendar::days_in_month(2019,  8) == 31);
    CHECK(calendar::days_in_month(2019,  9) == 30);
    CHECK(calendar::days_in_month(2019, 10) == 31);
    CHECK(calendar::days_in_month(2019, 11) == 30);
    CHECK(calendar::days_in_month(2019, 12) == 31);
    CHECK(calendar::days_in_month(2020,  1) == 31);
    CHECK(calendar::days_in_month(2020,  2) == 29);
    CHECK(calendar::days_in_month(2020,  3) == 31);
    CHECK(calendar::days_in_month(2020,  4) == 30);
    CHECK(calendar::days_in_month(2020,  5) == 31);
    CHECK(calendar::days_in_month(2020,  6) == 30);
    CHECK(calendar::days_in_month(2020,  7) == 31);
    CHECK(calendar::days_in_month(2020,  8) == 31);
    CHECK(calendar::days_in_month(2020,  9) == 30);
    CHECK(calendar::days_in_month(2020, 10) == 31);
    CHECK(calendar::days_in_month(2020, 11) == 30);
    CHECK(calendar::days_in_month(2020, 12) == 31);
}

TEST_CASE("days in year")
{
    CHECK(calendar::days_in_year(2019) == 365);
    CHECK(calendar::days_in_year(2020) == 366);
    CHECK(calendar::days_in_year(2021) == 365);
}
