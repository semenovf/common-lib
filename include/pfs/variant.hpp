////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019, 2020 Vladislav Trifochkin
//
// This file is part of [pfs-common](https://github.com/semenovf/pfs-common) library.
//
// Changelog:
//      2019.12.17 Initial version
//      2020.06.11 Pre-C++17 implementation replaced with stx::variant
////////////////////////////////////////////////////////////////////////////////
#pragma once

#if !defined(PFS_NO_STD_VARIANT)
#   include <variant>
#else
#   ifndef STX_NAMESPACE_NAME
#       define STX_NAMESPACE_NAME pfs
#   endif

#   define STX_NO_STD_VARIANT

#   include "3rdparty/stx/variant.hpp"

#endif // !PFS_NO_STD_VARIANT
