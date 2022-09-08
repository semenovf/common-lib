////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021,2022 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2022.05.10 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "assert.hpp"
#include "integer.hpp"
#include <type_traits>
#include <cstdint>
#include <cstdlib>

namespace pfs {

struct from_chars_result
{
    char const * ptr;
    std::errc ec;
};

template <typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
inline from_chars_result
from_chars (char const * first, char const * last, T & value, int base = 10)
{
    PFS__ASSERT(2 <= base && base <= 36, "base not in [2, 36]");
    auto result = parse_integer(first, last, value, base);
    return from_chars_result{result.ptr, result.ec};
}

} // namespace pfs
