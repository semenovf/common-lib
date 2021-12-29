////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2021.05.08 Initial version
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <cstdint>

#if defined(__has_include) // C++17
#   if __has_include(<source_location>) \
            && ((defined(_WIN32) || defined(_WIN64)) && defined(__cpp_consteval))
#       include <source_location>
namespace pfs {
        using source_location = std::source_location;
} // namespace pfs
#       define PFS_NO_STD_SOURCE_LOCATION 0
#       define PFS_CURRENT_SOURCE_LOCATION pfs::source_location::current()
#   elif __cplusplus > 201103L
#       if __GNUC__ && __has_include(<experimental/source_location>)
#           include <experimental/source_location>
namespace pfs {
            using source_location = std::experimental::fundamentals_v2::source_location;
} // namespace pfs
#           define PFS_NO_STD_SOURCE_LOCATION 0
#           define PFS_CURRENT_SOURCE_LOCATION pfs::source_location::current()
#       endif
#   endif
#endif

namespace pfs {

#ifndef PFS_NO_STD_SOURCE_LOCATION
#   define PFS_NO_STD_SOURCE_LOCATION 1

#   if _MSC_VER
#       define PFS_CURRENT_SOURCE_LOCATION source_location{__LINE__, __FILE__, __FUNCSIG__}
#   else
#       define PFS_CURRENT_SOURCE_LOCATION source_location{__LINE__, __FILE__, __PRETTY_FUNCTION__}
#   endif

class source_location
{
    std::uint_least32_t _line {0};
    std::uint_least32_t _column {0};
    char const * _file_name {nullptr};
    char const * _function_name {nullptr};

public:
    source_location () noexcept = default;
    source_location (source_location const & other) = default;
    source_location (source_location && other) noexcept = default;

    source_location (std::uint_least32_t line
            , char const * file_name
            , char const * function_name
            , std::uint_least32_t column = 0)
        : _line(line)
        , _column(column)
        , _file_name(file_name)
        , _function_name(function_name)
    {}

    std::uint_least32_t line () const noexcept
    {
        return _line;
    }

    std::uint_least32_t column () const noexcept
    {
        return _column;
    }

    char const * file_name () const noexcept
    {
        return _file_name;
    }

    char const * function_name () const noexcept
    {
        return _function_name;
    }
};

#endif

} // namespace pfs
