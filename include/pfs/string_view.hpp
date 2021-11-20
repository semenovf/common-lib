////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020-2021 Vladislav Trifochkin
//
// This file is part of [common-lib](https://github.com/semenovf/common-lib) library.
//
// Changelog:
//      2020.06.11 Initial version.
//      2021.11.23 Refactored excluding use of external cmake script.
////////////////////////////////////////////////////////////////////////////////
#pragma once

#if (defined(__cplusplus) && __cplusplus >= 201703L                \
        && defined(__has_include) && __has_include(<string_view>)) \
        || (defined(_MSC_VER) && __cplusplus >= 201703L)
#   define PFS_STD_STRING_VIEW_ENABLED 1
#   include <string_view>
namespace pfs {
    template <typename CharT, typename Traits = std::char_traits<CharT>>
    using basic_string_view = std::basic_string_view<CharT, Traits>;
    using string_view       = basic_string_view<char>;
    using wstring_view      = basic_string_view<wchar_t>;
    using u16string_view    = basic_string_view<std::char16_t>;
    using u32string_view    = basic_string_view<std::char32_t>;
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
#endif
