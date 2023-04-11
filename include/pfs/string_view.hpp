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
#include <cstring>

#if (defined(__cplusplus) && __cplusplus >= 201703L                \
        && defined(__has_include) && __has_include(<string_view>)) \
        || (defined(_MSC_VER) && __cplusplus >= 201703L)
#   define PFS_HAVE_STD_STRING_VIEW 1 // DEPRECATED
#   define PFS__HAVE_STD_STRING_VIEW 1
#   include <string>
#   include <string_view>
namespace pfs {
    using string_view = std::string_view;
} // namespace pfs
#else
#   ifndef STX_NAMESPACE_NAME
#       define STX_NAMESPACE_NAME pfs
#   endif

#   define STX_NO_STD_STRING_VIEW
#   include "3rdparty/stx/string_view.hpp"

    namespace pfs {
        // Avoid `undefined reference to `std::basic_string_view<char, std::char_traits<char> >::npos`
        template <typename CharT, typename Traits>
        constexpr typename basic_string_view<CharT, Traits>::size_type
        basic_string_view<CharT, Traits>::npos;
    } // namespace pfs

    using string_view = pfs::string_view;

    namespace std {
        template <>
        struct hash<pfs::string_view>
        {
            std::size_t operator () (pfs::string_view sv) const noexcept
            {
                std::size_t result = 0;
                std::hash<pfs::string_view::value_type> hasher;

                for (auto & ch: sv)
                    result = result * 31 + hasher(ch);

                return result;
            }
        };
    } // namespace std
#endif

// #ifndef PFS__HAVE_STD_STRING_VIEW
namespace pfs {
// #endif

inline std::string to_string (string_view sw)
{
    return std::string(sw.begin(), sw.end());
}

inline bool starts_with (string_view const & str, string_view::value_type ch)
{
    return str.size() >= 1 && str.front() == ch;
}

inline bool starts_with (string_view const & str, char const * p)
{
    // NOTE Not optimal
    auto len = std::strlen(p);
    return str.size() >= len && str.compare(0, len, p) == 0;
}

inline bool starts_with (string_view const & str, std::string const & prefix)
{
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

inline bool starts_with (string_view const & str, string_view const & prefix)
{
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

inline bool ends_with (string_view const & str, string_view::value_type ch)
{
    return str.size() >= 1 && str.back() == ch;
}

inline bool ends_with (string_view const & str, char const * p)
{
    // NOTE Not optimal
    auto len = std::strlen(p);
    return str.size() >= len && str.compare(str.size() - len, string_view::npos, p) == 0;
}

inline bool ends_with (string_view const & str, std::string const & suffix)
{
    return str.size() >= suffix.size() 
        && str.compare(str.size() - suffix.size(), string_view::npos, suffix) == 0;
}

inline bool ends_with (string_view const & str, string_view const & suffix)
{
    return str.size() >= suffix.size() 
        && str.compare(str.size() - suffix.size(), string_view::npos, suffix) == 0;
}

// #ifndef PFS__HAVE_STD_STRING_VIEW
} // namespace pfs
// #endif
