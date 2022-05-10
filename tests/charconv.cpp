////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2021.10.13 Initial version.
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define ANKERL_NANOBENCH_IMPLEMENT
#include "doctest.h"
#include "pfs/charconv.hpp"

// TODO Need more tests

TEST_CASE("convert chars to integers") {
    CHECK_EQ(pfs::to_int<int>("42"), 42);
    CHECK_EQ(pfs::to_int<int>("-42"), -42);
}
