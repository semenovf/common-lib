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

#if (defined(__cplusplus) && __cplusplus >= 201703L                \
        && defined(__has_include) && __has_include(<string_view>)) \
        || (defined(_MSC_VER) && __cplusplus >= 201703L)
#   define PFS_HAVE_STD_STRING_VIEW 1
#   include <string_view>
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

    template <>
    struct std::hash<pfs::string_view>
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
#endif
