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
using date = calendar::date;

TEST_CASE("comapre") {
    CHECK(date{2019, 11, 27} == date{2019, 11, 27});
    CHECK(date{2019, 11, 27} != date{2019, 11, 26});
    CHECK(date{2019, 11, 27} <  date{2019, 11, 28});
    CHECK(date{2019, 11, 27} <= date{2019, 11, 27});
    CHECK(date{2019, 11, 27} <= date{2019, 11, 28});
    CHECK(date{2019, 11, 27} >  date{2019, 11, 26});
    CHECK(date{2019, 11, 27} >= date{2019, 11, 27});
    CHECK(date{2019, 11, 27} >= date{2019, 11, 26});
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
    CHECK(date::null() == date{});

    CHECK(date::min().julian_day()
        == calendar::make_julian_day(std::numeric_limits<int>::min(), 1, 1));

    // FIXME Failed on Windows and on Linux
    // Linux: 783312487794 == 784354017364
#if __FIXME__
    CHECK(date::max().julian_day()
        == calendar::make_julian_day(std::numeric_limits<int>::max(), 12, 31));
#endif

    CHECK(date::epoch() == date{1970, 1, 1});
}

TEST_CASE("julian day")
{
    CHECK(date{0, 0, 0} == date{});
    CHECK(calendar::make_julian_day(1, 1, 1) == 1721424);

    // FIXME
//     int y, m, d;
//     pfs::date::from_julian_day(1721424, & y, & m, & d);
//     TEST_OK2(y == 1 && m == 1 && d == 1, "JD(1721424) is 01.01.0001");

    // Changes from Julian to Gregorian between October 4/15, 1582.
    CHECK(calendar::make_julian_day(1582, 10, 4)  == 2299160);
    CHECK(calendar::make_julian_day(1582, 10, 5)  == date{}.julian_day()); // invalid
    CHECK(calendar::make_julian_day(1582, 10, 6)  == date{}.julian_day()); // invalid
    CHECK(calendar::make_julian_day(1582, 10, 7)  == date{}.julian_day()); // invalid
    CHECK(calendar::make_julian_day(1582, 10, 8)  == date{}.julian_day()); // invalid
    CHECK(calendar::make_julian_day(1582, 10, 9)  == date{}.julian_day()); // invalid
    CHECK(calendar::make_julian_day(1582, 10, 10) == date{}.julian_day()); // invalid
    CHECK(calendar::make_julian_day(1582, 10, 11) == date{}.julian_day()); // invalid
    CHECK(calendar::make_julian_day(1582, 10, 12) == date{}.julian_day()); // invalid
    CHECK(calendar::make_julian_day(1582, 10, 13) == date{}.julian_day()); // invalid
    CHECK(calendar::make_julian_day(1582, 10, 14) == date{}.julian_day()); // invalid
    CHECK(calendar::make_julian_day(1582, 10, 15) == 2299161);

    CHECK(calendar::make_julian_day(1582, 10, 3) == 2299159);
    CHECK(calendar::make_julian_day(1582, 10, 16) == 2299162);

    CHECK(calendar::make_julian_day(1600, 1, 1) == 2305448);

    CHECK(calendar::make_julian_day(2013, 9, 13) == 2456549);
    CHECK(calendar::make_julian_day(-2013, 9, 13) == 986431);

    CHECK(date{2013, 9, 13}.days_to(date{  2013,  9, 13}) == 0);
    CHECK(date{2013, 9, 13}.days_to(date{  2013,  9, 12}) == -1);
    CHECK(date{2013, 9, 13}.days_to(date{  2013,  6,  5}) == -100);
    CHECK(date{2013, 9, 13}.days_to(date{  2010, 12, 18}) == -1000);
    CHECK(date{2013, 9, 13}.days_to(date{  1986,  4, 28}) == -10000);
    CHECK(date{2013, 9, 13}.days_to(date{  1739, 11, 29}) == -100000);
    CHECK(date{2013, 9, 13}.days_to(date{  -726, 10, 25}) == -1000000);
    CHECK(date{2013, 9, 13}.days_to(date{-25366,  2, 27}) == -10000000);

    date d1{2013, 9, 13};
    d1 = d1.add_days(-10000000);

    //pfs::date::fromJulianDay(d1.julianDay(), & y, & m, & d);
    //std::cout << "13.09.2013 - 10000000 days is " << d << "." << m << "." << y << std::endl;
    //long_t jd = pfs::date::julianDay(y, m, d);
    //TEST_OK(jd == d1.julianDay());

    date d2{-25366, 2, 27};
    CHECK_MESSAGE(d1 != date::null(), "13.9.2013 - 10000000 is a valid date and it is 27.2.25366 BC");
    CHECK_MESSAGE(d2 != date::null(), "27.2.25366 BC is valid date");
    CHECK_FALSE_MESSAGE((date{-25365, 2, 30} != date::null()), "30.2.25365 BC is invalid date");

    // 4.10.1582 + 1 day = 15.10.1582
    CHECK(date{1582, 10, 4}.add_days(1) == date{1582, 10, 15});
    CHECK(date{1582, 10, 15}.add_days(-1) == date{1582, 10, 4});

    CHECK(date{2020, 2, 28}.julian_day() == 2458908);
    CHECK(date{2020, 2, 29}.julian_day() == 2458909);
    CHECK(date{2020, 3,  1}.julian_day() == 2458910);
    CHECK(date{2020, 6, 20}.julian_day() == 2459021);

    auto di = calendar::decompose(2458909);
    CHECK(di.year == 2020);
    CHECK(di.month == 2);
    CHECK(di.day_of_month == 29);
    CHECK(di.day_of_week == 6);
    CHECK(di.day_of_year == 31 + 29);
}

// From Qt's QDate class reference
TEST_CASE("valid")
{
    CHECK_MESSAGE((date{2002, 5, 17} != date::null()), "17.05.2002 is a valid date"); // true
    CHECK_MESSAGE((date{2002, 2, 30} == date::null()), "30.02.2002 is not a valid date: Feb 30 does not exist");
    CHECK_MESSAGE((date{2004, 2, 29} != date::null()), "29.02.2004 is a valid date: 2004 is a leap year");
    CHECK_MESSAGE((date{2000, 2, 29} != date::null()), "29.02.2004 is a valid date: 2000 is a leap year");
    CHECK_MESSAGE((date{2006, 2, 29} == date::null()), "29.02.2006 is not a valid date: 2006 is not a leap year");
    CHECK_MESSAGE((date{2100, 2, 29} == date::null()), "29.02.2100 is not a valid date: 2100 is not a leap year");
    CHECK_MESSAGE((date{1202, 6,  6} != date::null()), "06.06.1202 is a valid date: even though 1202 is pre-Gregorian");

    intmax_t jd = intmax_t {2456575};
    auto d = calendar::decompose(jd);
    auto year  = d.year;
    auto month = d.month;
    auto day   = d.day_of_month;

    std::stringstream ss;
    ss << "9.10.2013 == " << jd;
    CHECK_MESSAGE((date{2013, 10, 9}.julian_day() == jd), ss.str().c_str());

    ss.clear();
    ss << jd << " (9.10.2013) == " << day << '.' << month << '.' << year;
    CHECK_MESSAGE((date{year, month, day}.julian_day() == jd), ss.str().c_str());
}

// From Qt's QDate class reference
TEST_CASE("periods")
{
    date d1{1995, 5, 17}; // May 17, 1995
    date d2{1995, 5, 20}; // May 20, 1995
    CHECK_MESSAGE((d1.days_to(d2) == 3), "Days bitween 17.05.1995 and 20.05.1995 is 3");
    CHECK_MESSAGE((d2.days_to(d1) == -3), "Days bitween 20.05.1995 and 17.05.1995 is -3");
}

TEST_CASE("stringify")
{
    using std::to_string;

    CHECK(to_string(date{2020, 2, 28}, std::string("{:%Y-%m-%d}")) == std::string("2020-02-28"));
    CHECK(to_string(date{2020, 2, 29}, std::string("{:%Y-%m-%d}")) == std::string("2020-02-29"));
    CHECK(to_string(date{2020, 3,  1}, std::string("{:%Y-%m-%d}")) == std::string("2020-03-01"));
    CHECK(to_string(date{2020, 6, 20}, std::string("{:%Y-%m-%d}")) == std::string("2020-06-20"));

    CHECK(to_string(date{2013, 11, 28}, std::string("{:%Y-%m-%d}")) == std::string("2013-11-28"));

    // TODO May be need uniformity in output for MSVC and g++
#if defined(_WIN32) || defined(_WIN64)
    CHECK(to_string(date{   1,  2,  3}, std::string("{:%Y-%m-%d}")) == std::string("0001-02-03"));
#else
    CHECK(to_string(date{   1,  2,  3}, std::string("{:%Y-%m-%d}")) == std::string("1-02-03"));
#endif

    CHECK(to_string(date{2013, 11, 28}) == std::string("2013-11-28"));

    // TODO May be need uniformity in output for MSVC and g++
#if defined(_WIN32) || defined(_WIN64)
    CHECK(to_string(date{   1,  2,  3}) == std::string("0001-02-03"));
#else
    CHECK(to_string(date{   1,  2,  3}) == std::string("1-02-03"));
#endif
}

TEST_CASE("add months")
{
    CHECK(date{31, 01, 2013}.add_months(1) == date{29, 02, 2013});
    CHECK(date{31, 01, 2013}.add_months(-2) == date{30, 11, 2012});
}

TEST_CASE("add years")
{
    // TODO
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
