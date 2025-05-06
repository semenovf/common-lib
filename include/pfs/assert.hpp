////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021,2022 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2022.05.08 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "error.hpp"
#include "fmt.hpp"
#include <exception>
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

#ifndef PFS__TERMINATE
#   define PFS__TERMINATE(condition, message)                     \
        ((condition)                                              \
            ? (void)0                                             \
            : ::pfs::assert_fail(__FILE__, __LINE__, (message)))
#endif

#ifndef PFS__THROW_UNEXPECTED
#   define PFS__THROW_UNEXPECTED(condition, message)              \
        ((condition)                                              \
            ? (void)0                                             \
            : throw ::pfs::error {                                \
                  make_error_code(::pfs::errc::unexpected_error)  \
                , fmt::format("{} at {}:{}", message, __FILE__, __LINE__)})
#endif

} // namespace pfs
