////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2023 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2023.03.29 Initial version
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <chrono>

constexpr std::chrono::microseconds operator ""_us (unsigned long long us)
{
    return std::chrono::microseconds(us);
}
constexpr std::chrono::duration<long double, std::micro> operator ""_us (long double us)
{
    return std::chrono::duration<long double, std::micro>(us);
}

constexpr std::chrono::milliseconds operator ""_ms (unsigned long long ms)
{
    return std::chrono::milliseconds(ms);
}

constexpr std::chrono::duration<long double, std::milli> operator ""_ms (long double ms)
{
    return std::chrono::duration<long double, std::milli>(ms);
}

constexpr std::chrono::seconds operator ""_s (unsigned long long s)
{
    return std::chrono::seconds(s);
}

constexpr std::chrono::duration<long double> operator ""_s(long double s)
{
    return std::chrono::duration<long double>(s);
}
