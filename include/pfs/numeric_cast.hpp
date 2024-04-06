////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021-2023 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2023.06.28 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <limits>
#include <stdexcept>
#include <type_traits>

namespace pfs {

template <typename T, typename S
    , typename std::enable_if<std::is_integral<S>::value
        && std::is_same<T, S>::value, int>::type = 0>
#if __cplusplus >= 201703L
    T constexpr numeric_cast (S const value)
#else
    T numeric_cast (S const value)
#endif
{
    return value;
}

template <typename T, typename S
    , typename std::enable_if<std::is_integral<S>::value
        && !std::is_same<T, S>::value
        && std::is_signed<T>::value
        && std::is_signed<S>::value, int>::type = 0>
#if __cplusplus >= 201703L
    T constexpr numeric_cast (S const value)
#else
    T numeric_cast (S const value)
#endif
{
    if (value > (std::numeric_limits<T>::max)())
        throw std::overflow_error("numeric_cast");

    if (value < std::numeric_limits<T>::min())
        throw std::underflow_error("numeric_cast");

    return static_cast<T>(value);
}

template <typename T, typename S
    , typename std::enable_if<std::is_integral<S>::value
        && !std::is_same<T, S>::value
        && std::is_unsigned<T>::value
        && std::is_unsigned<S>::value, int>::type = 0>
#if __cplusplus >= 201703L
    T constexpr numeric_cast (S const value)
#else
    T numeric_cast (S const value)
#endif
{
    if (value > (std::numeric_limits<T>::max)())
        throw std::overflow_error("numeric_cast");

    return static_cast<T>(value);
}

template <typename T, typename S
    , typename std::enable_if<std::is_integral<S>::value
        && !std::is_same<T, S>::value
        && std::is_unsigned<T>::value
        && std::is_signed<S>::value, int>::type = 0>
#if __cplusplus >= 201703L
    T constexpr numeric_cast (S const value)
#else
    T numeric_cast (S const value)
#endif
{
    // Avoid signed-unsigned comparison (-1 < 0U => false) problem
    // Consider zero is minimum limit for all unsigned integers
    //if (value < std::numeric_limits<T>::min())
    if (value < 0)
        throw std::underflow_error("numeric_cast");

    if (static_cast<std::intmax_t>(value) > (std::numeric_limits<T>::max)())
        throw std::overflow_error("numeric_cast");

    return static_cast<T>(value);
}

template <typename T, typename S
    , typename std::enable_if<std::is_integral<S>::value
        && !std::is_same<T, S>::value
        && std::is_signed<T>::value
        && std::is_unsigned<S>::value, int>::type = 0>
#if __cplusplus >= 201703L
    T constexpr numeric_cast (S const value)
#else
    T numeric_cast (S const value)
#endif
{
    if (value > (std::numeric_limits<T>::max)())
        throw std::overflow_error("numeric_cast");

    return static_cast<T>(value);
}

} // namespace pfs
