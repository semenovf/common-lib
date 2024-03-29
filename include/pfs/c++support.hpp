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

#   if __has_cpp_attribute(deprecated)
#       define PFS__DEPRECATED [[deprecated]]
#   endif

#   if __has_cpp_attribute(maybe_unused)
#       define PFS__MAYBE_UNUSED [[maybe_unused]]
#   endif
#endif

#ifndef PFS__NODISCARD
#   define PFS__NODISCARD
#endif

#ifndef PFS__DEPRECATED
#   define PFS__DEPRECATED
#endif

#ifndef PFS__MAYBE_UNUSED
#   define PFS__MAYBE_UNUSED
#endif
