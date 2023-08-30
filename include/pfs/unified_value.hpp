////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021-2023 Vladislav Trifochkin
//
// This file is part of `common-lib` library.
//
// Changelog:
//      2021.12.22 Initial version (debby-lib).
//      2023.08.23 Moved from debby-lib.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "pfs/string_view.hpp"
#include "pfs/type_traits.hpp"
#include "pfs/variant.hpp"
#include <string>
#include <vector>
#include <cstdint>

namespace pfs {

using blob_t  = std::vector<std::uint8_t>;
using basic_value_t = pfs::variant<
      std::nullptr_t
    , bool
    , std::intmax_t
    , float
    , double
    , blob_t        // bytes sequence
    , std::string>; // utf-8 encoded string

struct unified_value: public basic_value_t
{
    using string_view = pfs::string_view;

    unified_value () : basic_value_t(nullptr) {}

    unified_value (std::nullptr_t)
        : basic_value_t(nullptr)
    {}

    template <typename T>
    unified_value (T x, typename std::enable_if<std::is_same<typename std::decay<T>::type, bool>::value>::type * = 0)
        : basic_value_t(static_cast<bool>(x))
    {}

    /**
     * Construct @c unified_value from any integral type (char, int, etc).
     */
    template <typename T>
    unified_value (T x, typename std::enable_if<std::is_integral<T>::value
        && !std::is_same<T, bool>::value>::type * = 0)
        : basic_value_t(static_cast<std::intmax_t>(x))
    {}

    /**
     * Construct unified_value from single precision floatig point type (float).
     */
    unified_value (float x)
        : basic_value_t(x)
    {}

    /**
     * Construct unified_value from double precision floatig point type (double).
     */
    unified_value (double x)
        : basic_value_t(x)
    {}

    /**
     * Construct unified_value from string.
     */
    unified_value (std::string const & x)
        : basic_value_t(x)
    {}

    /**
     * Construct unified_value from string.
     */
    unified_value (char const * x)
        : basic_value_t(x ? std::string{x} : std::string{})
    {}

    /**
     * Construct unified_value from character sequence with length @a len.
     */
    unified_value (char const * x, std::size_t len)
        : basic_value_t(x && len > 0 ? std::string{x, len} : std::string{})
    {}

    /**
     * Construct unified_value from string_view.
     */
    unified_value (string_view x)
        : basic_value_t(pfs::to_string(x))
    {}

    /**
     * Construct unified_value from blob.
     */
    unified_value (blob_t const & x)
        : basic_value_t(x)
    {}

    template <typename T>
    static typename std::enable_if<std::is_same<std::nullptr_t,T>::value, unified_value>::type
    make_zero ()
    {
        return unified_value{};
    }

    template <typename T>
    static typename std::enable_if<std::is_same<typename std::decay<T>::type, bool>::value, unified_value>::type
    make_zero ()
    {
        return unified_value{false};
    }

    template <typename T>
    static typename std::enable_if<std::is_integral<T>::value
        && !std::is_same<typename std::decay<T>::type, bool>::value, unified_value>::type
    make_zero ()
    {
        return unified_value{static_cast<std::intmax_t>(0)};
    }

    template <typename T>
    static typename std::enable_if<std::is_floating_point<T>::value, unified_value>::type
    make_zero ()
    {
        return unified_value{T{0}};
    }

    template <typename T>
    static typename std::enable_if<std::is_same<typename std::decay<T>::type, std::string>::value, unified_value>::type
    make_zero ()
    {
        return unified_value{std::string{}};
    }

    template <typename T>
    static typename std::enable_if<std::is_same<typename std::decay<T>::type, string_view>::value, unified_value>::type
    make_zero ()
    {
        return unified_value{std::string{}};
    }

    template <typename T>
    static typename std::enable_if<std::is_same<typename std::decay<T>::type, blob_t>::value, unified_value>::type
    make_zero ()
    {
        return unified_value{blob_t{}};
    }
};

template <typename T>
inline typename std::enable_if<std::is_same<T, std::nullptr_t>::value, T>::type *
get_if (unified_value * u)
{
    return nullptr;
}

template <typename T>
inline typename std::enable_if<std::is_same<typename std::decay<T>::type, bool>::value, T>::type *
get_if (unified_value * u)
{
    return pfs::get_if<bool>(u);
}

template <typename T>
inline typename std::enable_if<std::is_integral<T>::value
    && !std::is_same<T, bool>::value, std::intmax_t>::type *
get_if (unified_value * u)
{
    return pfs::get_if<std::intmax_t>(u);
}

template <typename T>
inline typename std::enable_if<std::is_same<typename std::decay<T>::type, float>::value, T>::type *
get_if (unified_value * u)
{
    return pfs::get_if<float>(u);
}

template <typename T>
inline typename std::enable_if<std::is_same<typename std::decay<T>::type, double>::value, T>::type *
get_if (unified_value * u)
{
    return pfs::get_if<double>(u);
}

template <typename T>
inline typename std::enable_if<std::is_same<typename std::decay<T>::type, std::string>::value, T>::type *
get_if (unified_value * u)
{
    return pfs::get_if<std::string>(u);
}

template <typename T>
inline typename std::enable_if<std::is_same<typename std::decay<T>::type, blob_t>::value, T>::type *
get_if (unified_value * u)
{
    return pfs::get_if<blob_t>(u);
}

} // namespace pfs
