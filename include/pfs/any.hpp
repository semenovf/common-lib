////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020-2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2020.06.11 Initial version.
//      2021.11.20 Refactored excluding use of external cmake script.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "bits/compiler.h"

#if (defined(__cplusplus) && __cplusplus >= 201703L        \
        && defined(__has_include) && __has_include(<any>)) \
        || (defined(_MSC_VER) && __cplusplus >= 201703L)
#   define PFS_HAVE_STD_ANY 1
#   include <any>
#else
#   ifndef STX_NAMESPACE_NAME
#       define STX_NAMESPACE_NAME pfs
#   endif

// Avoid gcc warning in 3rdparty/stx/any.hpp:395
// warning: placement new constructing an object of type ‘T’ {aka ‘big_type’}
// and size ‘288’ in a region of type ‘pfs::any::storage_union::stack_storage_t’
// {aka ‘std::aligned_storage<16, 8>::type’} and size ‘16’ [-Wplacement-new=]
#   if PFS__COMPILER_GCC
#       pragma GCC diagnostic push
#       pragma GCC diagnostic ignored "-Wplacement-new"
#   endif

#   define STX_NO_STD_ANY
#   include "3rdparty/stx/any.hpp"
#   if PFS__COMPILER_GCC
#       pragma GCC diagnostic pop
#   endif
#endif
