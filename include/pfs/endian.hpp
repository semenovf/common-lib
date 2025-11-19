////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2021.10.14 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "namespace.hpp"
#include "bits/compiler.h"
#include "bits/endian.h"
#include "byteswap.hpp"
#include <type_traits>
#include <cstring>

PFS__NAMESPACE_BEGIN

enum class endian
{
      little = __ORDER_LITTLE_ENDIAN__
    , big    = __ORDER_BIG_ENDIAN__
//  , pdp    = __ORDER_PDP_ENDIAN__ // Unsupported
#if defined(__BYTE_ORDER__)
#   if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    , native = little
#   elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    , native = big
#   endif
#else
#   error "Can't determine or unsupported the endianness. An unique moment to add this compiler to the list of lucky ones"
#endif
    , network = big
};

/**
 * Convert from network to native order
 */

// Disable warning: ISO C++ does not support ‘__int128’ for ‘type name’ [-Wpedantic]
#if defined(PFS__HAS_INT128) && defined(PFS__COMPILER_GCC)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wpedantic"
#endif

template <typename T>
inline
typename std::enable_if_t<std::is_arithmetic<T>::value
#if defined(PFS__HAS_INT128)
        || std::is_same<std::decay_t<T>, __int128>::value
        || std::is_same<std::decay_t<T>, unsigned __int128>::value
#endif
    , T>
to_native_order (T const & x)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return byteswap(x);
#   elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return x;
#endif
}

/**
 * Convert from native to network order
 */
template <typename T>
inline typename std::enable_if_t<std::is_arithmetic<T>::value
#if defined(PFS__HAS_INT128)
        || std::is_same<std::decay_t<T>, __int128>::value
        || std::is_same<std::decay_t<T>, unsigned __int128>::value
#endif
    , T>
to_network_order (T const & x)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return byteswap(x);
#   elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return x;
#endif
}

#if defined(PFS__HAS_INT128) && defined(PFS__COMPILER_GCC)
#   pragma GCC diagnostic pop
#endif

PFS__NAMESPACE_END
