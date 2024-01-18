////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2024 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2024.01.18 Initial version.
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/unordered_erase.hpp"


TEST_CASE("default") {
    std::vector<int> v {1, 2, 3, 4, 5};

    CHECK_EQ(v.size(), 5);
    CHECK_EQ(v[2], 3);

    pfs::unordered_erase(v, 2);

    // 1, 2, 5, 4
    CHECK_EQ(v.size(), 4);
    CHECK_EQ(v[2], 5);

    pfs::unordered_erase(v, v.begin() + 2);

    // 1, 2, 4
    CHECK_EQ(v.size(), 3);
    CHECK_EQ(v[2], 4);

    pfs::unordered_erase(v, v.begin());

    // 4, 2
    CHECK_EQ(v.size(), 2);
    CHECK_EQ(v[0], 4);
    CHECK_EQ(v[1], 2);
}

