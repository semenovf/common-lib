////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of [common-lib](https://github.com/semenovf/common-lib) library.
//
// Changelog:
//      2020.06.11 Initial version
////////////////////////////////////////////////////////////////////////////////
#pragma once
#if !defined(PFS_NO_STD_STRING_VIEW)
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

}
#endif
