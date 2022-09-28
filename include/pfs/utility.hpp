////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2022 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2022.09.28 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <type_traits>
#include <utility>

namespace pfs {

// `to_underlying` since C++23
// https://en.cppreference.com/w/cpp/utility/to_underlying

template <typename Enum>
constexpr typename std::underlying_type<Enum>::type to_underlying (Enum e) noexcept
{
    return static_cast<typename std::underlying_type<Enum>::type>(e);
}

} // namespace pfs
