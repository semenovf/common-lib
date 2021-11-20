////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Vladislav Trifochkin
//
// This file is part of [common-lib](https://github.com/semenovf/common-lib) library.
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

#   define STX_NO_STD_VARIANT
#   include "3rdparty/stx/variant.hpp"
#endif
