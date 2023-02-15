////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2021.11.20 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "error.hpp"
#include "fmt.hpp"
#include <chrono>
#include <string>
#include <utility>
#include <cctype>
#include <cstring>
#include <ctime>
#include <type_traits>

namespace pfs {

using clock_type = std::chrono::system_clock;

struct date_time {
    int year;
    int month;
    int day;
    int hours;
    int minutes;
    int seconds;
    int millis;
};

/**
 * UTC offset in seconds
 */
inline std::time_t utc_offset ()
{
    std::time_t now = std::time(nullptr);
    std::tm utc_time;
    std::tm local_time;

#if _MSC_VER
    ::gmtime_s(& utc_time, & now);
#else
    std::tm * utc = std::gmtime(& now);
    memcpy(& utc_time, utc, sizeof(std::tm));
#endif

#if _MSC_VER
    ::localtime_s(& local_time, & now);
#else
    std::tm * local = std::localtime(& now);
    memcpy(& local_time, local, sizeof(std::tm));
#endif

    return (std::mktime(& local_time) - std::mktime(& utc_time));
}

/**
 * Returns string representation of UTC offset in format [+-]HHMM
 */
inline std::string stringify_utc_offset (std::time_t off)
{
    int sign = off < 0 ? -1 : 1;
    off *= sign;
    off /= 60;            // seconds
    auto min = off % 60;
    auto hour = off / 60;

    std::string result;
    result.reserve(5);

    result.append(1, sign > 0 ? '+' : '-');

    if (hour < 10)
        result.append(1, '0');

    result.append(std::to_string(hour));

    if (min < 10)
        result.append(1, '0');

    result.append(std::to_string(min));

    return result;
}

/**
 * Basic time point
 */
template <typename Derived>
class basic_time_point: public std::chrono::time_point<clock_type>
{
protected:
    using time_point = std::chrono::time_point<clock_type>;
    using base_class = time_point;

protected:
    basic_time_point (time_point const & t) : base_class(t) {}

public:
    basic_time_point () : base_class() {}
    basic_time_point (basic_time_point const & t) : base_class(t) {}
    explicit basic_time_point (base_class::duration const & d) : base_class(d) {}

    std::chrono::milliseconds to_millis () const noexcept
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(time_since_epoch());
    }

    date_time to_date_time () const noexcept
    {
        date_time dt {0,0,0,0,0,0,0};
        std::tm tm{};

        auto duration_since_epoch = time_since_epoch();
        auto seconds_since_epoch = std::chrono::duration_cast<std::chrono::seconds>(duration_since_epoch);
        auto millis_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(duration_since_epoch);
        std::time_t t = seconds_since_epoch.count();

        // Calculate floor accurate to seconds for negative duration since epoch
        if (duration_since_epoch.count() < 0) {
            if (millis_since_epoch.count() % 1000 != 0) {
                t -= 1;
            }
        }

#if _MSC_VER
        ::gmtime_s(& tm, & t);
#else
        std::memcpy(& tm, std::gmtime(& t), sizeof(std::tm));
#endif

        dt.year    = tm.tm_year + 1900;
        dt.month   = tm.tm_mon + 1;
        dt.day     = tm.tm_mday;
        dt.hours   = tm.tm_hour;
        dt.minutes = tm.tm_min;
        dt.seconds = tm.tm_sec;

        if (millis_since_epoch.count() % 1000 != 0) {
            if (millis_since_epoch.count() >= 0) {
                auto diff = millis_since_epoch
                    - std::chrono::duration_cast<std::chrono::milliseconds>(seconds_since_epoch);
                dt.millis = diff.count();
            } else {
                auto diff = millis_since_epoch
                    - std::chrono::duration_cast<std::chrono::milliseconds>(seconds_since_epoch);
                dt.millis = 1000 + diff.count();
            }
        }

        return dt;
    }

    ////////////////////////////////////////////////////////////////////////////
    // Arithmetic operators
    ////////////////////////////////////////////////////////////////////////////
    Derived & operator += (duration const & d)
    {
        base_class::operator += (d);
        return static_cast<Derived &>(*this);
    }

    Derived & operator -= (duration const & d)
    {
        base_class::operator -= (d);
        return static_cast<Derived &>(*this);
    }

    template <typename Rep, typename Period>
    friend Derived operator + (Derived const & pt
        , std::chrono::duration<Rep, Period> const & d)
    {
        return Derived{std::chrono::operator + (pt, d)};
    }

    template <typename Rep, typename Period>
    friend Derived operator + (std::chrono::duration<Rep, Period> const & d
        , Derived const & pt)
    {
        return Derived{std::chrono::operator + (d, pt)};
    }

    template <typename Rep, typename Period>
    friend Derived operator - (Derived const & pt
        , std::chrono::duration<Rep, Period> const & d)
    {
        return Derived{std::chrono::operator - (pt, d)};
    }

    template <typename Rep, typename Period>
    friend Derived operator - (Derived const & lhs
        , Derived const & rhs)
    {
        return Derived{std::chrono::operator - (lhs, rhs)};
    }

public: // static
    static Derived make (int year, int mon, int day, int hour, int min
        , int sec, int millis, std::error_code & ec) noexcept
    {
        return make(year, mon, day, hour, min, sec, millis, 0, 0, ec);
    }

    static Derived make (int year, int mon, int day, int hour, int min
        , int sec, int millis, int hour_offset = 0, int min_offset = 0)
    {
        std::error_code ec;
        auto t = make(year, mon, day, hour, min, sec, millis, hour_offset, min_offset, ec);

        if (ec)
            throw error{ec};

        return t;
    }

protected:
    static Derived make (int year, int mon, int day, int hour, int min
        , int sec, int millis, int hour_offset, int min_offset
        , std::error_code & ec) noexcept
    {
        auto offset_sign = hour_offset >= 0 ? 1 : -1;
        auto abs_hour_offset = hour_offset >= 0 ? hour_offset : -1 * hour_offset;

        auto success (mon > 0 && mon <= 12);
        success = success && (day > 0 && day <= 31);
        success = success && (hour >= 0 && hour < 24);
        success = success && (min >= 0 && min < 60);
        success = success && (sec >= 0 && sec < 60);
        success = success && (hour_offset >= -12 && hour_offset <= 14);
        success = success && (min_offset >= 0 && min_offset < 60);

        if (hour_offset >= 0) {
            success = success && (hour_offset * 3600 + min_offset * 60 <= 14 * 3600);
        } else {
            success = success && (hour_offset * 3600 - min_offset * 60 >= -12 * 3600);
        }

        if (!success) {
            ec = make_error_code(std::errc::invalid_argument);
            return Derived{};
        }

        std::tm tm;
        std::memset(& tm, 0, sizeof(std::tm));
        tm.tm_sec   = sec;
        tm.tm_min   = min;
        tm.tm_hour  = hour;
        tm.tm_mday  = day;
        tm.tm_mon   = mon - 1;
        tm.tm_year  = year - 1900;

        // Make
        std::time_t seconds_since_epoch = std::mktime(& tm);

        // Adjust by UTC offset since std::mktime created UTC based time.
        seconds_since_epoch += utc_offset();

        // Time offset in seconds
        int utc_offset = abs_hour_offset * 3600 + min_offset * 60;

        if (offset_sign > 0) {
            seconds_since_epoch -= utc_offset;
        } else {
            seconds_since_epoch += utc_offset;
        }

        auto tp = std::chrono::system_clock::from_time_t(seconds_since_epoch);
        tp += std::chrono::milliseconds{millis};
        return Derived{tp.time_since_epoch()};
    }
};

/**
 * Time point representation in UTC.
 */
class utc_time_point: public basic_time_point<utc_time_point>
{
    using base_class = basic_time_point<utc_time_point>;

public:
    using base_class::make;

    utc_time_point (): base_class() {}
    utc_time_point (base_class::time_point const & t) : base_class(t) {}

    template <typename Rep, typename Period>
    explicit utc_time_point (std::chrono::duration<Rep, Period> const & d)
        : base_class(std::chrono::duration_cast<base_class::duration>(d))
    {}

    std::string to_iso8601 () const
    {
        auto dt = to_date_time();
        return fmt::format("{}-{:02}-{:02}T{:02}:{:02}:{:02}.{:03}+0000"
            , dt.year, dt.month, dt.day, dt.hours, dt.minutes, dt.seconds
            , dt.millis);
    }

public: // static
    /**
     * Returns current time point.
     */
    static utc_time_point now ()
    {
        auto moment = clock_type::now();
        return utc_time_point{moment.time_since_epoch()};
    }

    /**
     * Makes UTC time point from date and time components.
     *
     * @param year
     * @param mon Month from 1 to 12 inclusive.
     * @param day Day of month from 1 to 31 inclusive.
     * @param hour Hours from 0 to 23 inclusive.
     * @param min Minutes from 0 to 59 inclusive.
     * @param sec Seconds from 0 to 59 inclusive.
     * @param millis Milliseconds.
     * @param hour_offset Offset in hours from -12 to 14 inclusive (when @a min_offset is zero).
     * @param min_offset Minutes from 0 to 59 inclusive.
     * @param ec Error code. Assigns @c std::errc::invalid_argument to @a ec on
     *        error.
     */
    static utc_time_point make (int year, int mon, int day, int hour, int min
        , int sec, int millis, int hour_offset, int min_offset
        , std::error_code & ec) noexcept
    {
        return base_class::make(year, mon, day, hour, min, sec, millis
            , hour_offset, min_offset, ec);
    }

    /**
     * Converts ISO 8601 standard string representation
     * (in formats YYYY-mm-ddTHH:MM:SS.SSS+ZZZZ or YYYY-mm-ddTHH:MM:SSZ)
     * of time to time point in UTC.
     *
     * @param s String representaion of time pount in ISO 8601.
     * @param ec Error code. Assigns @c std::errc::invalid_argument to @a ec on
     *        error.
     */
    static utc_time_point from_iso8601 (std::string const & s, std::error_code & ec) noexcept
    {
        int year = 0;
        int mon  = 0;
        int day  = 0;
        int hour = 0;
        int min  = 0;
        int sec  = 0;
        int millis = 0;

        int hour_offset = 0;
        int min_offset = 0;
        int offset_sign = 1;

        // 0         1         2
        // 0123456789012345678901234567
        // 2021-11-22T11:33:42.999+0500;
        // 2021-10-13T08:06:59Z
        //
        bool success = (std::isdigit(s[0]) && std::isdigit(s[1]) && std::isdigit(s[2]) && std::isdigit(s[3])
            && s[4] == '-'
            && std::isdigit(s[5]) && std::isdigit(s[6])
            && s[7] == '-'
            && std::isdigit(s[8]) && std::isdigit(s[9])
            && s[10] == 'T'
            && std::isdigit(s[11]) && std::isdigit(s[12])
            && s[13] == ':'
            && std::isdigit(s[14]) && std::isdigit(s[15])
            && s[16] == ':'
            && std::isdigit(s[17]) && std::isdigit(s[18]));

        if (!success) {
            ec = make_error_code(std::errc::invalid_argument);
            return utc_time_point{};
        }

        year = (s[0] - '0') * 1000 + (s[1] - '0') * 100 + (s[2] - '0') * 10 + (s[3] - '0');
        mon  = (s[5] - '0') * 10 + (s[6] - '0');
        day  = (s[8] - '0') * 10 + (s[9] - '0');
        hour = (s[11] - '0') * 10 + (s[12] - '0');
        min  = (s[14] - '0') * 10 + (s[15] - '0');
        sec  = (s[17] - '0') * 10 + (s[18] - '0');

        if (s[19] == 'Z') {
            ;
        } else {
            success = s[19] == '.'
                && std::isdigit(s[20]) && std::isdigit(s[21]) && std::isdigit(s[22])
                && (s[23] == '-' || s[23] == '+')
                && std::isdigit(s[24]) && std::isdigit(s[25]) && std::isdigit(s[26]) && std::isdigit(s[27]);

            if (success) {
                millis = (s[20] - '0') * 100 + (s[21] - '0') * 10 + (s[22] - '0');
                offset_sign = s[23] == '-' ? -1 : 1;
                hour_offset = (s[24] - '0') * 10 + (s[25] - '0');
                min_offset = (s[26] - '0') * 10 + (s[27] - '0');
            }
        }

        if (!success) {
            ec = make_error_code(std::errc::invalid_argument);
            return utc_time_point{};
        }

        return make(year, mon, day, hour, min, sec, millis, offset_sign * hour_offset, min_offset, ec);
    }

    /**
     * Converts ISO 8601 standard string representation
     * (in formats YYYY-mm-ddTHH:MM:SS.SSS+ZZZZ or YYYY-mm-ddTHH:MM:SSZ)
     * of time to time point in UTC.
     *
     * @throw pfs::error {std::errc::invalid_argument} on parse error.
     */
    static utc_time_point from_iso8601 (std::string const & s)
    {
        std::error_code ec;
        auto t = from_iso8601(s, ec);

        if (ec)
            throw error{ec};

        return t;
    }
};

/**
 * Time point representation in local time.
 */
class local_time_point: public basic_time_point<local_time_point>
{
    using base_class = basic_time_point<local_time_point>;

public:
    using base_class::make;

    local_time_point (): base_class() {}
    local_time_point (base_class::time_point const & t) : base_class(t) {}

    template <typename Rep, typename Period>
    explicit local_time_point (std::chrono::duration<Rep, Period> const & d)
        : base_class(std::chrono::duration_cast<base_class::duration>(d))
    {}

    std::string to_iso8601 () const
    {
        auto dt = to_date_time();
        return fmt::format("{}-{:02}-{:02}T{:02}:{:02}:{:02}.{:03}{}"
            , dt.year, dt.month, dt.day, dt.hours, dt.minutes, dt.seconds
            , dt.millis
            , stringify_utc_offset(utc_offset()));
    }

public: // static
    /**
     * Returns current time point.
     */
    static local_time_point now ()
    {
        auto moment = clock_type::now().time_since_epoch();
        moment += std::chrono::seconds{utc_offset()};
        return local_time_point{moment};
    }
};

/**
 * Returns current time point in UTC.
 */
inline utc_time_point current_utc_time_point ()
{
    return utc_time_point::now();
}

/**
 * Returns current local time point.
 */
inline local_time_point current_local_time_point ()
{
    return local_time_point::now();
}

/**
 * Convert local time point to UTC time point.
 */
inline utc_time_point utc_time_point_cast (local_time_point const & t)
{
    auto since_epoch = t.time_since_epoch();
    auto off = utc_offset();
    since_epoch -= std::chrono::seconds{off};
    return utc_time_point{since_epoch};
}

/**
 * Convert UTC time point to local time point.
 */
inline local_time_point local_time_point_cast (utc_time_point const & t)
{
    auto since_epoch = t.time_since_epoch();
    auto off = utc_offset();
    since_epoch += std::chrono::seconds{off};
    return local_time_point{since_epoch};
}

/**
 * String representation of @c utc_time_point.
 */
inline std::string to_string (utc_time_point const & t)
{
    return t.to_iso8601();
}

/**
 * String representation of @c local_time_point.
 */
inline std::string to_string (local_time_point const & t)
{
    return t.to_iso8601();
}

using utc_time = utc_time_point;
using local_time = local_time_point;

} // namespace pfs
