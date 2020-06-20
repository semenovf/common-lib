////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of [pfs-common](https://github.com/semenovf/pfs-common) library.
//
// Changelog:
//      2019.06.20 Initial version (inspired from date class https://github.com/semenovf/pfs)
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "compare.hpp"
#include <array>
#include <limits>
#include <locale>
#include <tuple>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <ctime>

namespace pfs {
namespace calendar {

constexpr intmax_t long_long_ago = std::numeric_limits<intmax_t>::min();
constexpr std::array<int, 13> const __days_in_month { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

struct date_info
{
    int year;
    int month;        // [1-12]
    int day_of_month; // [1-31]
    int day_of_week;  // Day of week [1-7]
    int day_of_year;  // Days in year [1-365(366)], 366 - for leap year
};

/*
 * Integer Division (Floor function)
 * http://mathworld.wolfram.com/IntegerDivision.html
 */
inline int floor_div (int a, int b)
{
    return (a - (a < 0 ? b - 1 : 0)) / b;
}

/**
 * Algorithm (http://en.wikipedia.org/wiki/Leap_year):
 *
 * if year is divisible by 400 then
 *      is_leap_year
 * else if year is divisible by 100 then
 *      not_leap_year
 * else if year is divisible by 4 then
 *      is_leap_year
 * else
 *      not_leap_year
 */
inline bool is_leap_year (intmax_t year)
{
    if (year < 1) // There is no year 0 in Gregorian calendar
        ++year;
    return year % 400 == 0 || (year % 4 == 0 && year % 100 != 0);
}

/*
* Checks if the specified date is valid
*/
inline constexpr bool valid (int year, int month, int day)
{
    return year == 0
        ? false
        : (day > 0 && month > 0 && month <= 12)
            && (day <= __days_in_month[month]
                || (day == 29 && month == 2 && is_leap_year(year)));
}

/**
 * Calculates Julian Day (JD) from Gregorian calendar date
 */
inline intmax_t make_julian_day (int year, int month, int day)
{
    if (month < 1 || month > 12)
        return long_long_ago;

    if (day < 1 || day > 31)
        return long_long_ago;

    if (year < 0) // there is no 0 year
        ++year;

    int      a = floor_div(14 - month, 12);
    intmax_t y = intmax_t(year) + 4800 - a;
    int      m = month + 12 * a - 3;

    // Gregorian calendar: >= 15.10.1582
    if (year > 1582 || (year == 1582 && (month > 10 || (month == 10 && day >= 15)))) {
        return day + floor_div(153 * m + 2, 5)
            + 365 * y
            + floor_div(y, 4)
            - floor_div(y, 100)
            + floor_div(y, 400)
            - 32045;
    } else if (year < 1582 || (year == 1582 && (month < 10 || (month == 10 && day <= 4)))) {
        // Julian calendar: <= 4.10.1582

        return day + floor_div(153 * m + 2, 5)
            + 365 * y
            + floor_div(y, 4)
            - 32083;
    }

    return long_long_ago;
}

/**
 * @brief Calculates Gregorian calendar date from Julian Day (JD)
 *
 * @param julian_day JD value.
 *
 * @return Date decomposed into @c struct @c tm fields.
 *
 * @note see http://www.tondering.dk/claus/cal/julperiod.php.
 */
date_info decompose (intmax_t julian_day)
{
    intmax_t b = 0;
    intmax_t c = 0;

    // Gregorian calendar
    if (julian_day >= 2299161) {
        intmax_t a = julian_day + 32044;
        b = floor_div(4 * a + 3, 146097);
        c = a - floor_div(146097 * b, 4);
    } else {
        b = 0;
        c = julian_day + 32082;
    }

    intmax_t d = floor_div(4 * c + 3, 1461);
    intmax_t e = c - floor_div(1461 * d, 4);
    intmax_t m = floor_div(5 * e + 2, 153);

    date_info result;
    result.day_of_month = e - floor_div(153 * m + 2, 5) + 1;
    result.month = m + 3 - 12 * floor_div(m, 10);
    result.year = 100 * b + d - 4800 + floor_div(m, 10);

    if (result.year <= 0)
        --result.year;

    // Day of week (0 = Monday to 6 = Sunday)
    result.day_of_week = (julian_day >= 0)
        ? (julian_day % 7) + 1
        : ((julian_day + 1) % 7) + 7;

    result.day_of_year = julian_day - make_julian_day(result.year, 1, 1) + 1;

    return result;
}

class date final: public compare_operations
{
public:
    using value_type = intmax_t;

private:
    value_type _julian_day {long_long_ago};

private:
    explicit constexpr date (value_type jd)
        : _julian_day(jd)
    {}

public:
    date () = default;
    date (date const &) = default;
    date (date &&) = default;
    date & operator = (date const &) = default;
    date & operator = (date &&) = default;

    date (int year, int month, int day)
    {
        if (valid(year, month, day))
            _julian_day = make_julian_day(year, month, day);
        else
            _julian_day = long_long_ago;
    }

    /**
     * @return Julian day.
     */
    value_type julian_day () const noexcept
    {
        return _julian_day;
    }

    date add_days (int ndays) const
    {
        return date{_julian_day + ndays};
    }

    date add_months (int nmonths) const
    {
        // Note: algorithm adopted from QDate::addMonths

        if (_julian_day == long_long_ago)
            return date{};

        if (!nmonths)
            return *this;

        auto di = decompose(_julian_day);

        int start_year = di.year;

        while (nmonths != 0) {
            if (nmonths < 0 && nmonths + 12 <= 0) {
                --di.year;
                nmonths += 12;
            } else if (nmonths < 0) {
                di.month += nmonths;
                nmonths = 0;

                if (di.month <= 0) {
                    --di.year;
                    di.month += 12;
                }
            } else if (nmonths - 12 >= 0) {
                ++di.year;
                nmonths -= 12;
            } else if (di.month == 12) {
                ++di.year;
                di.month = 0;
            } else {
                di.month += nmonths;
                nmonths = 0;

                if (di.month > 12) {
                    ++di.year;
                    di.month -= 12;
                }
            }
        }

        // special cases: transition the year through 0
        if (start_year > 0 && di.year <= 0)      // decreasing months
            --di.year;
        else if (start_year < 0 && di.year >= 0) // increasing months
            ++di.year;

        return date(di.year, di.month, di.day_of_month);
    }

    date add_years (int nyears) const
    {
        // Note: algorithm adopted from QDate::addYears

        if (_julian_day == long_long_ago)
            return date{};

        if (!nyears)
            return *this;

        auto di = decompose(_julian_day);

        int start_year = di.year;
        di.year += nyears;

        // special cases: transition the year through 0
        if (start_year > 0 && di.year <= 0)      // decreasing months
            --di.year;
        else if (start_year < 0 && di.year >= 0) // increasing months
            ++di.year;

        return date(di.year, di.month, di.day_of_month);
    }

    /**
     * @return Number of days from @a d.
     */
    value_type days_to (date const & d) const
    {
        return d._julian_day - _julian_day;
    }

    /**
     * @return The year.
     * @note Use decompose() function instead if need another components at once
     */
    int year () const
    {
        if (_julian_day == long_long_ago)
            return 0;

        auto di = decompose(_julian_day);
        return di.year;
    }

    /**
     * @return The month.
     * @note Use decompose() function instead if need another components at once
     */
    int month () const
    {
        if (_julian_day == long_long_ago)
            return 0;

        auto di = decompose(_julian_day);
        return di.month;
    }

    /**
     * @return The day of month.
     * @note Use decompose() function instead if need another components at once
     */
    int day_of_month () const
    {
        if (_julian_day == long_long_ago)
            return 0;

        auto di = decompose(_julian_day);
        return di.day_of_month;
    }

    /**
     * @return The day of the year (0 to 364 or 365 on leap years).
     * @note Use decompose() function instead if need another components at once
     */
    int day_of_year () const
    {
        auto d = decompose(_julian_day);
        return d.day_of_year;
    }

    /**
     * @return The day of the week (1 to 7).
     * @note Use decompose() function instead if need another components at once
     */
    int day_of_week () const
    {
        auto d = decompose(_julian_day);
        return d.day_of_week;
    }

    //
    // Compare
    //
    int compare (date const & rhs) const noexcept
    {
        return _julian_day == rhs._julian_day
            ? 0
            : _julian_day < rhs._julian_day
                ? -1
                : +1;
    }

    bool operator == (date const & rhs) const noexcept
    {
        return compare(rhs) == 0;
    }

    bool operator < (date const & rhs) const noexcept
    {
        return compare(rhs) < 0;
    }

public:
    static constexpr const date null () noexcept
    {
        return date{};
    };

    // date {1970, 1, 1}
    static constexpr const date epoch () noexcept
    {
        return date{2440588L};
    }

    // date {std::numeric_value<int>::min(), 1, 1}
    static constexpr const date min () noexcept
    {
        return date{-784366681008L};
    }

    // date {std::numeric_value<int>::max(), 12, 31}
    static constexpr const date max () noexcept
    {
        return date{+783312487794L};
    }
};

inline date_info decompose (date const & d)
{
    return decompose(d.julian_day());
}

/**
 * @return The calculated number of days in the month (28 to 31) for specified
 *      year and month.
 */
inline int days_in_month (int year, int month)
{
    return  (month == 2 && is_leap_year(year))
        ? 29
        : __days_in_month[month];
}

/**
 * @return The number of days in the year (365 or 366) for specified year.
 */
inline int days_in_year (int year)
{
    return is_leap_year(year) ? 366 : 365;
}

/**
 * @param format The format of conversion (see std::strftime specific format,
 *      https://en.cppreference.com/w/cpp/chrono/c/strftime)
 */
inline std::string to_string (date const & d, std::string const & format)
{
    if (format.empty())
        return std::string{};

    auto di = decompose(d.julian_day());
    struct tm timeinfo;
    std::memset(& timeinfo, 0, sizeof(timeinfo));

    timeinfo.tm_mday = di.day_of_month;    // Day [1-31]
    timeinfo.tm_mon  = di.month - 1;       // Month [0-11]
    timeinfo.tm_year = di.year - 1900;     // Year - 1900
    timeinfo.tm_wday = di.day_of_week - 1; // Day of week [0-6]
    timeinfo.tm_yday = di.day_of_year - 1; // Days in year [0-365]

    // https://stackoverflow.com/questions/7935483/c-function-to-format-time-t-as-stdstring-buffer-length
    std::string result;
    result.resize(format.size());
    int len = std::strftime(& result[0], result.size(), format.c_str(), & timeinfo);

    while (len == 0) {
        result.resize(result.size()*2);
        len = strftime(& result[0], result.size(), format.c_str(), & timeinfo);
    }

    result.resize(len); //remove that trailing '\a'

    return result;
}

/**
 * @brief Converts date to string.
 *
 * @details The string format corresponds to the ISO 8601 specification,
 *          taking the form YYYY-MM-DD, where YYYY is the year,
 *          MM is the month of the year (between 01 and 12),
 *          and DD is the day of the month between 01 and 31.
 *
 * @return The date as string.
 */
inline std::string to_string (date const & d)
{
    return to_string(d, std::string{"%F"}); // equivalent to %H:%M:%S
}

}} // namespace pfs::calendar
