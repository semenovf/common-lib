////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2019.12.17 Initial version.
//      2020.06.11 Pre-C++17 implementation replaced with stx::variant
//      2021.11.20 Refactored excluding use of external cmake script.
////////////////////////////////////////////////////////////////////////////////
#pragma once

#if (defined(__cplusplus) && __cplusplus >= 201703L            \
        && defined(__has_include) && __has_include(<variant>)) \
        || (defined(_MSC_VER) && __cplusplus >= 201703L)
#   define PFS_HAVE_STD_VARIANT 1
#   include <variant>
#else
#   ifndef STX_NAMESPACE_NAME
#       define STX_NAMESPACE_NAME pfs
#   endif

#if _MSC_VER
// Disable warnings
// warning C4244: '=': conversion from 'ptrdiff_t' to '...', possible loss of data
// warning C4244: 'initializing': conversion from 'ptrdiff_t' to '...', possible loss of data
#   pragma warning(push)
#   pragma warning(disable:4244)
#endif

#   define STX_NO_STD_VARIANT
#   include "3rdparty/stx/variant.hpp"

#ifdef _MSC_VER
#   pragma warning(pop)
#endif

#endif
