////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2021.11.20 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "optional.hpp"
#include "pfs/fmt.hpp"
#include <chrono>
#include <string>
#include <utility>
#include <cctype>
#include <cstring>
#include <ctime>

namespace pfs {

using clock_type = std::chrono::system_clock;
using time_point = std::chrono::time_point<clock_type>;

/**
 * Time point representation in UTC.
 */
struct utc_time_point
{
    time_point value;
};

inline bool operator == (utc_time_point const & lhs, utc_time_point const & rhs)
{
    return lhs.value == rhs.value;
}

inline bool operator != (utc_time_point const & lhs, utc_time_point const & rhs)
{
    return lhs.value == rhs.value;
}

inline bool operator < (utc_time_point const & lhs, utc_time_point const & rhs)
{
    return lhs.value < rhs.value;
}

inline bool operator <= (utc_time_point const & lhs, utc_time_point const & rhs)
{
    return lhs.value <= rhs.value;
}

inline bool operator > (utc_time_point const & lhs, utc_time_point const & rhs)
{
    return lhs.value > rhs.value;
}

inline bool operator >= (utc_time_point const & lhs, utc_time_point const & rhs)
{
    return lhs.value >= rhs.value;
}

/**
 * UTC offset in seconds
 */
inline std::time_t utc_offset ()
{
    std::time_t now = std::time(nullptr);
    std::tm utc_time;
    std::tm local_time;

    std::tm * utc = std::gmtime(& now);
    memcpy(& utc_time, utc, sizeof(std::tm));

    std::tm * local = std::localtime(& now);
    memcpy(& local_time, local, sizeof(std::tm));

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
    int min = off % 60;
    int hour = off / 60;

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

inline std::chrono::milliseconds to_millis (time_point const & t)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(t.time_since_epoch());
}

inline std::chrono::milliseconds to_millis (utc_time_point const & t)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(t.value.time_since_epoch());
}

inline time_point from_millis (std::chrono::milliseconds const & millis)
{
    return time_point{millis};
}

/**
 * Returns current local time point with precision in milliseconds
 */
inline time_point current_time_point ()
{
    return std::chrono::time_point_cast<std::chrono::milliseconds>(clock_type::now());
}

/**
 * Returns current time point in UTC with precision in milliseconds
 */
inline utc_time_point current_utc_time_point ()
{
    auto now = current_time_point();
    auto off = utc_offset();
    now -= std::chrono::seconds{off};
    return utc_time_point{now};
}

/**
 * Convert local time point to UTC time point.
 */
inline utc_time_point to_utc_time_point (time_point const & t)
{
    auto millis = to_millis(t);
    auto off = utc_offset();
    millis -= std::chrono::seconds{off};
    return utc_time_point{from_millis(millis)};
}

/**
 * Convert UTC time point to local time point.
 */
inline time_point to_local_time_point (utc_time_point const & t)
{
    auto millis = to_millis(t.value);
    auto off = utc_offset();
    millis += std::chrono::seconds{off};
    return from_millis(millis);
}

/**
 * Represents time point as string according to ISO 8601 standard.
 *
 * @note May be used as Date and Time value in sqlite3.
 */
inline std::string to_iso8601 (time_point const & t)
{
    auto millis = to_millis(t).count() % 1000;
    return fmt::format("{0:%Y-%m-%dT%H:%M:%S}.{1:03}{0:%z}", t, millis);
}

inline std::string to_iso8601 (utc_time_point const & t)
{
    auto millis = to_millis(t).count() % 1000;
    return fmt::format("{0:%Y-%m-%dT%H:%M:%S}.{1:03}+0000", t.value, millis);
}

/**
 * Now it is same as to_iso8601()
 */
inline std::string to_string (time_point const & t)
{
    return to_iso8601(t);
}

inline std::string to_string (utc_time_point const & t)
{
    return to_iso8601(t);
}

/**
 * Converts ISO 8601 standard string representation
 * (in formats YYYY-mm-ddTHH:MM:SS.SSS+ZZZZ or YYYY-mm-ddTHH:MM:SSZ)
 * of time to time point in UTC.
 */
inline optional<utc_time_point> from_iso8601 (std::string const & s)
{
    time_point result;

    int year = 0;
    int mon  = 0;
    int day  = 0;
    int hour = 0;
    int min  = 0;
    int sec  = 0;
    int millis = 0;

    int hour_offset = 0;
    int min_offset = 0;

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

    if (success) {
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

                hour_offset = (s[24] - '0') * 10 + (s[25] - '0');
                min_offset = (s[26] - '0') * 10 + (s[27] - '0');
            }
        }

        success = success && (mon > 0 && mon <= 12);
        success = success && (day > 0 && day <= 31);
        success = success && (hour >= 0 && hour < 24);
        success = success && (min >= 0 && min < 60);
        success = success && (sec >= 0 && sec < 60);
        success = success && (hour_offset >= 0 && hour_offset <= 24);
        success = success && (min_offset >= 0 && min_offset < 60);
        success = success && (hour_offset * 100 + min_offset <= 2400);

        if (success) {
            std::tm tm;
            std::memset(& tm, 0, sizeof(std::tm));
            tm.tm_sec   = sec;
            tm.tm_min   = min;
            tm.tm_hour  = hour;
            tm.tm_mday  = day;
            tm.tm_mon   = mon - 1;
            tm.tm_year  = year - 1900;

            std::time_t seconds_since_epoch = std::mktime(& tm);
            success = seconds_since_epoch >= 0;

            if (success) {
                // Time offset in seconds
                int utc_offset = hour_offset * 3600 + min_offset * 60;

                if (s[23] == '+') {
                    if (seconds_since_epoch < utc_offset)
                        seconds_since_epoch = 0;
                    else
                        seconds_since_epoch -= utc_offset;
                } else {
                    seconds_since_epoch += utc_offset;
                }

                result = std::chrono::system_clock::from_time_t(seconds_since_epoch);
                result += std::chrono::milliseconds{millis};
            }
        }
    }

    return success
        ? optional<utc_time_point>(utc_time_point{result})
        : optional<utc_time_point>{};
}

} // namespace pfs
