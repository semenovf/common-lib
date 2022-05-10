////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021,2022 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2022.05.10 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "string_view.hpp"
#include <cstdint>
#include <cstdlib>

namespace pfs {

inline std::intmax_t __to_intmax (char const * begin
    , char const * end
    , bool & success
    , int radix)
{
    success = true;

    if (begin < end) {
        char * endptr = nullptr;
        errno = 0;

        if (sizeof(std::intmax_t) == sizeof(long int)) {
            auto n = std::strtol(begin, & endptr, radix);

            if (endptr == end && !errno)
                return static_cast<std::intmax_t>(n);
        } else {
            auto n = std::strtoll(begin, & endptr, radix);

            if (endptr == end && !errno)
                return static_cast<std::intmax_t>(n);
        }
    }

    success = false;

    return std::intmax_t{0};
}

inline std::uintmax_t __to_uintmax (char const * begin
    , char const * end
    , bool & success
    , int radix)
{
    success = true;

    if (begin < end) {
        char * endptr = nullptr;
        errno = 0;

        if (sizeof(std::uintmax_t) == sizeof(unsigned long int)) {
            auto n = std::strtoul(begin, & endptr, radix);

            if (endptr == end && !errno)
                return static_cast<std::uintmax_t>(n);
        } else {
            auto n = std::strtoull(begin, & endptr, radix);

            if (endptr == end && !errno)
                return static_cast<std::uintmax_t>(n);
        }
    }

    success = false;

    return std::uintmax_t{0};
}

template <typename IntT>
IntT __to_int (char const * begin
    , char const * end
    , IntT min_val
    , IntT max_val
    , bool * success
    , int radix)
{
    bool ok = true;
    auto n = __to_intmax(begin, end, ok, radix);

    if (ok && n >= min_val && n <= max_val) {
        if (success)
            *success = true;

        return static_cast<IntT>(n);
    }

    if (success)
        *success = false;

    return IntT{0};
}

template <typename UIntT>
UIntT __to_uint (char const * begin
    , char const * end
    , UIntT min_val
    , UIntT max_val
    , bool * success
    , int radix)
{
    bool ok = true;
    auto n = __to_uintmax(begin, end, ok, radix);

    if (ok && n >= min_val && n <= max_val) {
        if (success)
            *success = true;
        return static_cast<UIntT>(n);
    }

    if (success)
        *success = false;

    return UIntT{0};
}

template <typename IntT>
inline IntT to_int (char const * begin, char const * end
    , bool * success = nullptr, int radix = 10)
{
    return __to_int(begin, end
        , std::numeric_limits<IntT>::min()
        , std::numeric_limits<IntT>::max()
        , success, radix);
}

template <typename UIntT>
inline UIntT to_uint (char const * begin, char const * end
    , bool * success = nullptr, int radix = 10)
{
    return __to_uint(begin, end
        , std::numeric_limits<UIntT>::min()
        , std::numeric_limits<UIntT>::max()
        , success, radix);
}

template <typename IntT>
inline IntT to_int (char const * s, bool * success = nullptr, int radix = 10)
{
    return __to_int(s, s + std::strlen(s)
        , std::numeric_limits<IntT>::min()
        , std::numeric_limits<IntT>::max()
        , success, radix);
}

template <typename UIntT>
inline UIntT to_uint (char const * s, bool * success = nullptr, int radix = 10)
{
    return __to_uint(s, s + std::strlen(s)
        , std::numeric_limits<UIntT>::min()
        , std::numeric_limits<UIntT>::max()
        , success, radix);
}

template <typename IntT>
inline IntT to_int (string_view const & s, bool * success = nullptr, int radix = 10)
{
    return __to_int(s.data(), s.data() + s.size()
        , std::numeric_limits<IntT>::min()
        , std::numeric_limits<IntT>::max()
        , success, radix);
}

template <typename UIntT>
inline UIntT to_uint (string_view const & s, bool * success = nullptr, int radix = 10)
{
    return __to_uint(s.data(), s.data() + s.size()
        , std::numeric_limits<UIntT>::min()
        , std::numeric_limits<UIntT>::max()
        , success, radix);
}

///
template <typename IntT>
inline IntT to_int (std::string const & s, bool * success = nullptr, int radix = 10)
{
    return __to_int(s.data(), s.data() + s.size()
        , std::numeric_limits<IntT>::min()
        , std::numeric_limits<IntT>::max()
        , success, radix);
}

template <typename UIntT>
inline UIntT to_uint (std::string const & s, bool * success = nullptr, int radix = 10)
{
    return __to_uint(s.data(), s.data() + s.size()
        , std::numeric_limits<UIntT>::min()
        , std::numeric_limits<UIntT>::max()
        , success, radix);
}

} // namespace pfs
