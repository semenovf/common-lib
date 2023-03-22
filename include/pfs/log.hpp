////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2022 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2021.09.29 Initial version (netty-lib).
//      2022.03.29 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "bits/compiler.h"
#include "pfs/fmt.hpp"
#include <chrono>
#include <string>

#if __ANDROID__
#    include <android/log.h>
#endif

// https://stackoverflow.com/questions/5891221/variadic-macros-with-zero-arguments
//#define PFS__VA_ARGS(...) , ##__VA_ARGS__

inline std::string stringify_trace_time ()
{
    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
    using std::chrono::steady_clock;

    std::uint64_t msecs = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();

    int millis = msecs % 1000;
    std::uint64_t seconds = msecs / 1000;
    std::uint64_t hours   = seconds / 3600;
    seconds -= hours * 3600;
    std::uint64_t minutes = seconds / 60;
    seconds -= minutes * 60;

    //std::uint64_t sample = millis
    //    + (seconds + minutes * 60 + hours * 3600) * 1000;

    return fmt::format("{}:{:02}:{:02}.{:03}"
        , hours, minutes, seconds, millis);
}

#if __ANDROID__
//
    inline void __android_log_print_helper(int prio
        , std::string const & tag, std::string const & text)
    {
#   if PFS_COMPILER_CLANG
#       pragma clang diagnostic push
#       pragma clang diagnostic ignored "-Wformat-security"
        __android_log_print(prio, tag.c_str(), text.c_str());
#       pragma clang diagnostic pop
#   endif
    }

    inline void __android_log_print_helper(int prio
        , char const * tag, std::string const & text)
    {
#   if PFS_COMPILER_CLANG
#       pragma clang diagnostic push
#       pragma clang diagnostic ignored "-Wformat-security"
        __android_log_print(prio, tag, text.c_str());
#       pragma clang diagnostic pop
#   endif
    }

#   define LOGD(t, f, ...) __android_log_print_helper(ANDROID_LOG_DEBUG, t, fmt::format(f , ##__VA_ARGS__))
#   define LOGI(t, f, ...) __android_log_print_helper(ANDROID_LOG_INFO, t, fmt::format(f , ##__VA_ARGS__))
#   define LOGW(t, f, ...) __android_log_print_helper(ANDROID_LOG_WARN, t, fmt::format(f , ##__VA_ARGS__))
#   define LOGE(t, f, ...) __android_log_print_helper(ANDROID_LOG_ERROR, t, fmt::format(f , ##__VA_ARGS__))

#   ifdef PFS__LOG_LEVEL
#       if PFS__LOG_LEVEL >= 1
#           define LOG_TRACE_1(f, ...) __android_log_print_helper(             \
                  ANDROID_LOG_VERBOSE                                          \
                , "-- TRACE(1)", fmt::format(f , ##__VA_ARGS__).c_str())
#       endif

#       if PFS__LOG_LEVEL >= 2
#           define LOG_TRACE_2(f, ...) __android_log_print_helper(             \
                  ANDROID_LOG_VERBOSE                                          \
                , "-- TRACE(2)", fmt::format(f , ##__VA_ARGS__).c_str())
#       endif

#       if PFS__LOG_LEVEL >= 3
#           define LOG_TRACE_3(f, ...) __android_log_print_helper(             \
                  ANDROID_LOG_VERBOSE                                          \
                , "-- TRACE(3)", fmt::format(f , ##__VA_ARGS__).c_str())
#       endif
#   endif // PFS__LOG_LEVEL
#else  // __ANDROID__
#   define LOGD(t, f, ...) {                                               \
        fmt::print(stdout, "{} [D] {}: " f "\n"                            \
            , stringify_trace_time(), t , ##__VA_ARGS__); fflush(stdout);}
#   define LOGI(t, f, ...) {                                               \
        fmt::print(stdout, "{} [I] {}: " f "\n"                            \
            , stringify_trace_time(), t , ##__VA_ARGS__); fflush(stdout);}
#   define LOGW(t, f, ...) {                                               \
        fmt::print(stderr, "{} [W] {}: " f "\n"                            \
            , stringify_trace_time(), t , ##__VA_ARGS__); fflush(stderr);}
#   define LOGE(t, f, ...) {                                               \
        fmt::print(stderr, "{} [E] {}: " f "\n"                            \
            , stringify_trace_time(), t , ##__VA_ARGS__); fflush(stderr);}

#   ifdef PFS__LOG_LEVEL
#       if PFS__LOG_LEVEL >= 1
#           define LOG_TRACE_1(f, ...) {                                       \
                fmt::print(stdout, "{} -- TRACE(1): " f "\n"                   \
                    , stringify_trace_time() , ##__VA_ARGS__); fflush(stdout);}
#       endif

#       if PFS__LOG_LEVEL >= 2
#           define LOG_TRACE_2(f, ...) {                                       \
                fmt::print(stdout, "{} -- TRACE(2): " f "\n"                   \
                    , stringify_trace_time() , ##__VA_ARGS__); fflush(stdout);}
#       endif

#       if PFS__LOG_LEVEL >= 3
#           define LOG_TRACE_3(f, ...) {                                       \
              fmt::print(stdout, "{} -- TRACE(3): " f "\n"                     \
            , stringify_trace_time() , ##__VA_ARGS__); fflush(stdout);}
#       endif
#   endif // PFS__LOG_LEVEL
#endif // !__ANDROID__

#ifndef LOG_TRACE_1
#   define LOG_TRACE_1(format, ...)
#endif

#ifndef LOG_TRACE_2
#   define LOG_TRACE_2(format, ...)
#endif

#ifndef LOG_TRACE_3
#   define LOG_TRACE_3(format, ...)
#endif

#if __ANDROID__
#   define LOGEXP(t, x) __android_log_print_helper(ANDROID_LOG_ERROR, t, x.what())
#else
#   define LOGEXP(t, x) {                                                      \
              fmt::print(stderr, fmt::format("{} [E] {}: {}\n"                 \
            , stringify_trace_time(), t, x.what())); fflush(stderr);}
#endif
