////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021-2023 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2023.06.28 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "type_traits.hpp"
#include <limits>
#include <stdexcept>

namespace pfs {

template <typename T, typename U>
#if __cplusplus >= 201703L
constexpr
#else
inline
#endif
typename std::enable_if<std::is_integral<U>::value && is_same_decayed<T, U>::value, T>::type
numeric_cast (U const value)
{
    return value;
}

// signed(U) => signed(T)
template <typename T, typename U>
#if __cplusplus >= 201703L
constexpr
#else
inline
#endif
typename std::enable_if<is_signed_integer<T>::value && is_signed_integer<U>::value
        && !is_same_decayed<T, U>::value, T>::type
numeric_cast (U const value)
{
    if (value > (std::numeric_limits<T>::max)())
        throw std::overflow_error("numeric_cast");

    if (value < (std::numeric_limits<T>::min)())
        throw std::underflow_error("numeric_cast");

    return static_cast<T>(value);
}

// unsigned(U) => unsigned(T)
template <typename T, typename U>
#if __cplusplus >= 201703L
constexpr
#else
inline
#endif
typename std::enable_if<is_unsigned_integer<T>::value
        && is_unsigned_integer<U>::value
        && !is_same_decayed<T, U>::value, T>::type
numeric_cast (U const value)
{
    if (value > (std::numeric_limits<T>::max)())
        throw std::overflow_error("numeric_cast");

    return static_cast<T>(value);
}

// signed(U) => unsigned(T)
template <typename T, typename U>
// #if __cplusplus >= 201703L
// constexpr
// #else
// inline
// #endif
typename std::enable_if<is_unsigned_integer<T>::value && is_signed_integer<U>::value, T>::type
numeric_cast (U const value)
{
    if (value >= 0) {
        if (static_cast<std::uintmax_t>(value) > (std::numeric_limits<T>::max)())
            throw std::overflow_error("numeric_cast");
        return static_cast<T>(value);
    }

    // value < 0
    if (sizeof(T) < sizeof(U))
        throw std::underflow_error("numeric_cast");

    return static_cast<T>(value);
}

// unsigned(U) => signed(T)
template <typename T, typename U>
#if __cplusplus >= 201703L
constexpr
#else
inline
#endif
typename std::enable_if<is_signed_integer<T>::value && is_unsigned_integer<U>::value, T>::type
numeric_cast (U const value)
{
    if (value > static_cast<std::uintmax_t>((std::numeric_limits<T>::max)()))
        throw std::overflow_error("numeric_cast");

    return static_cast<T>(value);
}

// integral(U) => bool(T)
template <typename T, typename U>
#if __cplusplus >= 201703L
constexpr
#else
inline
#endif
typename std::enable_if<is_bool<T>::value && !is_bool<U>::value
    && std::is_integral<U>::value, bool>::type
numeric_cast (U const value)
{
    return static_cast<T>(value);
}

// bool(U) => integral(T)
template <typename T, typename U>
#if __cplusplus >= 201703L
constexpr
#else
inline
#endif
typename std::enable_if<std::is_integral<T>::value && !is_bool<T>::value && is_bool<U>::value, T>::type
numeric_cast (U const value)
{
    return static_cast<T>(value);
}

} // namespace pfs
