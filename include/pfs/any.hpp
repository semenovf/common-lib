////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of [pfs-common](https://github.com/semenovf/pfs-common) library.
//
// Changelog:
//      2020.06.11 Initial version
////////////////////////////////////////////////////////////////////////////////
#pragma once
#if !defined(PFS_NO_STD_ANY)
#   include <any>
#else
#   ifndef STX_NAMESPACE_NAME
#       define STX_NAMESPACE_NAME pfs
#   endif

#   define STX_NO_STD_ANY

// Avoid gcc warning in 3rdparty/stx/any.hpp:395
// warning: placement new constructing an object of type ‘T’ {aka ‘big_type’}
// and size ‘288’ in a region of type ‘pfs::any::storage_union::stack_storage_t’
// {aka ‘std::aligned_storage<16, 8>::type’} and size ‘16’ [-Wplacement-new=]
#pragma GCC diagnostic push
// #pragma GCC diagnostic ignored "-Wplacement-new"
#   include "3rdparty/stx/any.hpp"
#pragma GCC diagnostic pop

#endif
