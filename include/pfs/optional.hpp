////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of [common-lib](https://github.com/semenovf/common-lib) library.
//
// Changelog:
//      2020.06.11 Initial version
////////////////////////////////////////////////////////////////////////////////
#pragma once
#if !defined(PFS_NO_STD_OPTIONAL)
#   include <optional>

#   define PFS_HAVE_STD_OPTIONAL 1
#else
#   ifndef STX_NAMESPACE_NAME
#       define STX_NAMESPACE_NAME pfs
#   endif

#   define STX_NO_STD_OPTIONAL

#   include "3rdparty/stx/optional.hpp"

#   ifdef STX_HAVE_STD_OPTIONAL
#       define PFS_HAVE_STD_OPTIONAL 1
#   endif
#endif
