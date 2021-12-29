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

#if (defined(__cplusplus) && __cplusplus >= 201703L             \
        && defined(__has_include) && __has_include(<optional>)) \
        || (defined(_MSC_VER) && __cplusplus >= 201703L)
#   define PFS_HAVE_STD_OPTIONAL 1
#   include <optional>

    namespace pfs {
        using std::optional;
        using std::bad_optional_access;
        using std::nullopt_t;
        using std::nullopt;
        using std::make_optional;
        using std::in_place_t;
        using std::in_place;
    }
#else
#   ifndef STX_NAMESPACE_NAME
#       define STX_NAMESPACE_NAME pfs
#   endif

#   define STX_NO_STD_OPTIONAL
#   include "3rdparty/stx/optional.hpp"
#endif
