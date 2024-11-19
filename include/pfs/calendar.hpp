////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2019.06.20 Initial version (inspired from date class https://github.com/semenovf/pfs)
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "compare.hpp"
#include "error.hpp"
#include "fmt.hpp"
#include "i18n.hpp"
#include "integer.hpp"
#include <array>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <limits>
#include <locale>
#include <string>
#include <tuple>

namespace pfs {
namespace calendar {

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
template <typename IntT1, typename IntT2>
inline IntT1 floor_div (IntT1 a, IntT2 b)
{
    if (b == 0) {
        throw pfs::error {
              std::make_error_code(std::errc::invalid_argument)
            , tr::_("division by zero")
        };
    }

    return (a - (a < 0 ? IntT2{b} - 1 : 0)) / IntT2{b};
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
inline bool is_leap_year (int year)
{
    if (year < 1) // There is no year 0 in Gregorian calendar
        ++year;
    return year % 400 == 0 || (year % 4 == 0 && year % 100 != 0);
}

/**
 * @return The number of days in the month (28 to 31) for specified @a month.
 */
inline int days_in_month (int month)
{
    return std::array<int, 13> { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }[month];
}

/**
 * @return The calculated number of days in the month (28 to 31) for specified
 *      @a year and @a month.
 */
inline int days_in_month (int year, int month)
{
    return  (month == 2 && is_leap_year(year))
        ? 29
        : days_in_month(month);
}

/**
 * @return The number of days in the year (365 or 366) for specified year.
 */
inline int days_in_year (int year)
{
    return is_leap_year(year) ? 366 : 365;
}

/**
 * Determines the the of the week (Sakamoto's method)
 *
 * @return The day of the week (1 to 7).
 * @see https://en.wikipedia.org/wiki/Determination_of_the_day_of_the_week.
 */
inline int day_of_week (int y, int m, int d)
{
    static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};

    if (m < 3)
        y--;

    auto x = (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;
    return x == 0 ? 7 : x;
}

/**
 * Julian day representatio class
 */
class julian_day final: public compare_operations
{
public:
    using value_type = std::int64_t;

public:
    static constexpr value_type raw_min_day   = -784366681008L; // julian_day(INT_MIN, 1, 1)
    static constexpr value_type raw_max_day   =  784354017364L; // julian_day(INT_MAX, 12, 31)
    static constexpr value_type raw_epoch_day =       2440588L; // julian_day(1970, 1, 1)

private:
    value_type _value {raw_epoch_day};

private:
    explicit constexpr julian_day (value_type value)
        : _value(value)
    {}

public:
    /**
     * Constructs Julian day with day 1 January 4713 BC (native() returns zero)
     */
    julian_day () : julian_day(value_type{0}) {}

    julian_day (julian_day const &) = default;
    julian_day (julian_day &&) = default;
    julian_day & operator = (julian_day const &) = default;
    julian_day & operator = (julian_day &&) = default;

    /**
     * Constructs Julian Day (JD) from Gregorian calendar date.
     *
     * @throw pfs::error{std::errc::invalid_argument} if specified date is invalid.
     */
    julian_day (int year, int month, int day)
    {
        *this = make(year, month, day);
    }

    /**
    * @brief Calculates Gregorian calendar date from Julian Day (JD)
    * @param julian_day JD value.
    * @return Date decomposed into @c struct @c tm fields.
    * @note see http://www.tondering.dk/claus/cal/julperiod.php.
    */
    date_info decompose () const
    {
        value_type b = 0;
        value_type c = 0;

        // Gregorian calendar
        if (_value >= 2299161) {
            value_type a = _value + 32044;
            b = floor_div(4 * a + 3, 146097);
            c = a - floor_div(146097 * b, 4);
        } else {
            b = 0;
            c = _value + 32082;
        }

        value_type d = floor_div(4 * c + 3, 1461);
        value_type e = c - floor_div(1461 * d, 4);
        value_type m = floor_div(5 * e + 2, 153);

        date_info result;

        auto day_of_month = e - floor_div(153 * m + 2, 5) + 1;
        auto month = m + 3 - 12 * floor_div(m, 10);
        auto year = 100 * b + d - 4800 + floor_div(m, 10);

        result.day_of_month = static_cast<decltype(result.day_of_month)>(day_of_month);
        result.month = static_cast<decltype(result.month)>(month);
        result.year = static_cast<decltype(result.year)>(year);

        if (result.year <= 0)
            --result.year;

        // Day of week (0 = Monday to 6 = Sunday)
        result.day_of_week = (_value >= 0)
            ? (_value % 7) + 1
            : ((_value + 1) % 7) + 7;

        auto day_of_year = _value - make(result.year, 1, 1).native() + 1;

        result.day_of_year = static_cast<decltype(result.day_of_year)>(day_of_year);

        return result;
    }

    /**
     * @return Raw value for Julian day.
     */
    value_type native () const noexcept
    {
        return _value;
    }

    julian_day add_days (int ndays) const
    {
        auto value = sum_safe(_value, ndays);

        if (value > raw_epoch_day)
            throw std::overflow_error("add days");

        if (value < raw_min_day)
            throw std::underflow_error("add days");

        return julian_day{value};
    }

    julian_day add_months (int nmonths) const
    {
        // Note: algorithm adopted from QDate::addMonths

        if (nmonths == 0)
            return *this;

        auto di = decompose();

        int start_year = di.year;

        while (nmonths != 0) {
            if (nmonths < 0 && nmonths + 12 <= 0) {
                di.year = sum_safe(di.year, -1);
                nmonths = sum_safe(nmonths, 12);
            } else if (nmonths < 0) {
                di.month = sum_safe(di.month, nmonths);
                nmonths = 0;

                if (di.month <= 0) {
                    di.year = sum_safe(di.year,  -1);
                    di.month = sum_safe(di.month, 12);
                }
            } else if (nmonths - 12 >= 0) {
                di.year = sum_safe(di.year,  1);
                di.month = sum_safe(di.month, -12);
            } else if (di.month == 12) {
                di.year = sum_safe(di.year,  1);
                di.month = 0;
            } else {
                di.month = sum_safe(di.month, nmonths);
                nmonths = 0;

                if (di.month > 12) {
                    di.year = sum_safe(di.year,  1);
                    di.month = sum_safe(di.month, -12);
                }
            }
        }

        // special cases: transition the year through 0
        if (start_year > 0 && di.year <= 0)      // decreasing months
            di.year = sum_safe(di.year, -1);
        else if (start_year < 0 && di.year >= 0) // increasing months
            di.year = sum_safe(di.year,  1);

        return make_safe(di.year, di.month, di.day_of_month);
    }

    julian_day add_years (int nyears) const
    {
        // Note: algorithm adopted from QDate::addYears

        if (nyears == 0)
            return *this;

        auto di = decompose();

        int start_year = di.year;
        di.year = sum_safe(di.year, nyears);

        // special cases: transition the year through 0
        if (start_year > 0 && di.year <= 0)      // decreasing years
            di.year = sum_safe(di.year, -1);
        else if (start_year < 0 && di.year >= 0) // increasing years
            di.year = sum_safe(di.year, 1);

        return make_safe(di.year, di.month, di.day_of_month);
    }

    /**
     * @return Number of days from @a d.
     */
    value_type days_to (julian_day const & d) const
    {
        return d._value - _value;
    }

    /**
     * @return The year.
     * @note Use decompose() function instead if need another components at once
     */
    int year () const
    {
        return decompose().year;
    }

    /**
     * @return The month.
     * @note Use decompose() function instead if need another components at once
     */
    int month () const
    {
        return decompose().month;
    }

    /**
     * @return The day of month.
     * @note Use decompose() function instead if need another components at once
     */
    int day_of_month () const
    {
        return decompose().day_of_month;
    }

    /**
     * @return The day of the year (0 to 364 or 365 on leap years).
     * @note Use decompose() function instead if need another components at once
     */
    int day_of_year () const
    {
        return decompose().day_of_year;
    }

    /**
     * @return The day of the week (1 to 7).
     * @note Use decompose() function instead if need another components at once
     */
    int day_of_week () const
    {
        return decompose().day_of_week;
    }

    //
    // Compare
    //
    int compare (julian_day const & other) const noexcept
    {
        return _value == other._value ? 0 : _value < other._value ? -1 : +1;
    }

    bool operator == (julian_day const & other) const noexcept
    {
        return compare(other) == 0;
    }

    bool operator < (julian_day const & other) const noexcept
    {
        return compare(other) < 0;
    }

public:
    static julian_day make (value_type value, error * perr = nullptr)
    {
        if (value < raw_min_day || value > raw_max_day) {
            throw_or(perr, std::make_error_code(std::errc::invalid_argument), tr::_("bad Julian day"));
            return julian_day{};
        }

        return julian_day{value};
    }

    /**
     * Calculates Julian Day (JD) from Gregorian calendar date. If @a day is greater than maximum
     * days in specified @a month then value of @a day set to maximum value.
     *
     * @throw pfs::error{std::errc::invalid_argument} if specified date is invalid.
     */

    /**
     * Calculates Julian Day (JD) from Gregorian calendar date.
     *
     * @throw pfs::error{std::errc::invalid_argument} if specified date is invalid.
     */
    static julian_day make (int year, int month, int day, error * perr = nullptr)
    {
        if (!check_day(year, month, day)) {
            throw_or(perr, std::make_error_code(std::errc::invalid_argument), tr::_("bad date"));
            return julian_day{};
        }

        if (month < 1 || month > 12) {
            throw_or(perr, std::make_error_code(std::errc::invalid_argument), tr::_("bad month"));
            return julian_day{};
        }

        if (day < 1 || day > 31) {
            throw_or(perr, std::make_error_code(std::errc::invalid_argument), tr::_("bad day"));
            return julian_day{};
        }

        if (year < 0) // there is no 0 year
            ++year;

        int  a = floor_div(14 - month, 12);
        auto y = value_type{year} + 4800 - a;
        int  m = month + 12 * a - 3;

        // Gregorian calendar: >= 15.10.1582
        if (year > 1582 || (year == 1582 && (month > 10 || (month == 10 && day >= 15)))) {
            return julian_day {static_cast<value_type>(day) + floor_div(153 * m + 2, 5)
                + 365 * y
                + floor_div(y, 4)
                - floor_div(y, 100)
                + floor_div(y, 400)
                - 32045};
        } else if (year < 1582 || (year == 1582 && (month < 10 || (month == 10 && day <= 4)))) {
            // Julian calendar: <= 4.10.1582

            return julian_day {static_cast<value_type>(day) + floor_div(153 * m + 2, 5)
                + 365 * y
                + floor_div(y, 4)
                - 32083};
        }

        throw_or(perr, std::make_error_code(std::errc::invalid_argument), tr::_("bad date"));
        return julian_day{};
    }

    // julian_day {1970, 1, 1}
    static constexpr const julian_day epoch_day () noexcept
    {
        return julian_day{raw_epoch_day};
    }

    static constexpr const julian_day min_day () noexcept
    {
        return julian_day{raw_min_day};
    }

    static constexpr const julian_day max_day () noexcept
    {
        return julian_day{raw_max_day};
    }

private:
    /*
    * Checks if the specified date is valid
    */
    static constexpr bool check_day (int year, int month, int day)
    {
        return year == 0
            ? false
            : (day > 0 && month > 0 && month <= 12)
                && (day <= days_in_month(month)
                    || (day == 29 && month == 2 && is_leap_year(year)));
    }

    static julian_day make_safe (int year, int month, int day, error * perr = nullptr)
    {
        return make(year, month, (std::min)(day, days_in_month(year, month)), perr);
    }
};

/**
 * @param format The format of conversion (see std::strftime specific format,
 *      https://en.cppreference.com/w/cpp/chrono/c/strftime)
 */
inline std::string to_string (julian_day const & d, std::string const & format)
{
    if (format.empty())
        return std::string{};

    auto di = d.decompose();
    struct tm timeinfo;
    std::memset(& timeinfo, 0, sizeof(timeinfo));

    timeinfo.tm_mday = di.day_of_month;    // Day [1-31]
    timeinfo.tm_mon  = di.month - 1;       // Month [0-11]
    timeinfo.tm_year = di.year - 1900;     // Year - 1900
    timeinfo.tm_wday = di.day_of_week - 1; // Day of week [0-6]
    timeinfo.tm_yday = di.day_of_year - 1; // Days in year [0-365]

    //// https://stackoverflow.com/questions/7935483/c-function-to-format-time-t-as-stdstring-buffer-length
    //std::string result;
    //result.resize(format.size());
    //int len = std::strftime(& result[0], result.size(), format.c_str(), & timeinfo);

    //while (len == 0) {
    //    result.resize(result.size()*2);
    //    len = strftime(& result[0], result.size(), format.c_str(), & timeinfo);
    //}

    //result.resize(len); //remove that trailing '\a'

    auto result = fmt::format(format, timeinfo);
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
inline std::string to_string (julian_day const & d)
{
    return to_string(d, std::string("{:%F}")); // equivalent to %H:%M:%S
}

constexpr julian_day::value_type julian_day::raw_min_day;
constexpr julian_day::value_type julian_day::raw_max_day;
constexpr julian_day::value_type julian_day::raw_epoch_day;

}} // namespace pfs::calendar
