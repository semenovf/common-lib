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
#       define STX_NAMESPACE_NAME std
#   endif
#   include "3rdparty/stx/any.hpp"
#endif
