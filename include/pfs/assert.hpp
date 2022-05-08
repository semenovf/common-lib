////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021,2022 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2022.05.08 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <cstdio>

namespace pfs {

namespace {
inline void assert_fail (char const * file, int line, char const * message)
{
    std::fprintf(stderr, "%s:%d: assertion failed: %s\n", file, line, message);
    std::terminate();
}
} // namespace

#ifndef PFS__ASSERT
#   ifdef NDEBUG
#       define PFS__ASSERT(condition, message)
#   else
#       define PFS__ASSERT(condition, message)                        \
            ((condition)                                              \
                ? (void)0                                             \
                : ::pfs::assert_fail(__FILE__, __LINE__, (message)))
#   endif
#endif

} // namespace pfs
