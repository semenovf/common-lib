////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2023 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2023.11.27 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once

#ifdef __has_cpp_attribute
#   if __has_cpp_attribute(nodiscard)
#       define PFS__NODISCARD [[nodiscard]]
#   endif
#endif

#ifndef PFS__NODISCARD
#   define PFS__NODISCARD
#endif
