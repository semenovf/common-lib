////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of [pfs-common](https://github.com/semenovf/pfs-common) library.
//
// Changelog:
//      2019.10.21 Initial version
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <limits>
#include <cassert>

namespace pfs {

template <typename IntT1, typename IntT2>
inline IntT1 integral_cast (IntT2 a)
{
    assert(a >= std::numeric_limits<IntT1>::min()
        && a <= std::numeric_limits<IntT1>::max());

    return static_cast<IntT1>(a);
}

} // namespace pfs
