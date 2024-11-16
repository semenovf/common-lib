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
        && !std::is_same<typename std::decay<T>::type, bool>::value
        && !std::is_same<typename std::decay<S>::type, bool>::value
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

    if (value < (std::numeric_limits<T>::min)())
        throw std::underflow_error("numeric_cast");

    return static_cast<T>(value);
}

template <typename T, typename S
    , typename std::enable_if<std::is_integral<S>::value
        && !std::is_same<typename std::decay<T>::type, bool>::value
        && !std::is_same<typename std::decay<S>::type, bool>::value
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
        && !std::is_same<typename std::decay<T>::type, bool>::value
        && !std::is_same<typename std::decay<S>::type, bool>::value
        && !std::is_same<T, S>::value
        && std::is_unsigned<T>::value
        && std::is_signed<S>::value, int>::type = 0>
#if __cplusplus >= 201703L
    T constexpr numeric_cast (S const value)
#else
    T numeric_cast (S const value)
#endif
{
    if (static_cast<std::uintmax_t>(value) > (std::numeric_limits<T>::max)())
        throw std::overflow_error("numeric_cast");

    return static_cast<T>(value);
}

template <typename T, typename S
    , typename std::enable_if<std::is_integral<S>::value
        && !std::is_same<typename std::decay<T>::type, bool>::value
        && !std::is_same<typename std::decay<S>::type, bool>::value
        && !std::is_same<T, S>::value
        && std::is_signed<T>::value
        && std::is_unsigned<S>::value, int>::type = 0>
#if __cplusplus >= 201703L
    T constexpr numeric_cast (S const value)
#else
    T numeric_cast (S const value)
#endif
{
    if (value > static_cast<std::uintmax_t>((std::numeric_limits<T>::max)()))
        throw std::overflow_error("numeric_cast");

    return static_cast<T>(value);
}

template <typename T, typename U>
inline typename std::enable_if<std::is_same<typename std::decay<T>::type, bool>::value
    && std::is_integral<U>::value, bool>::type
numeric_cast (U const value)
{
    return static_cast<T>(value);
}

template <typename T, typename U>
inline typename std::enable_if<std::is_integral<T>::value
    && std::is_same<typename std::decay<U>::type, bool>::value, T>::type
numeric_cast (U const value)
{
    return static_cast<T>(value);
}

} // namespace pfs
