////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2023 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2023.11.20 Initial version.
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/integer.hpp"
#include <limits>

TEST_CASE("increment") {
    CHECK_THROWS_AS(pfs::sum_safe((std::numeric_limits<std::int8_t>::max)(), 1), std::overflow_error);
    CHECK_THROWS_AS(pfs::sum_safe((std::numeric_limits<std::int8_t>::min)(),-1), std::underflow_error);

    CHECK_EQ(pfs::sum_safe(41,  1), 42);
    CHECK_EQ(pfs::sum_safe(43, -1), 42);
}
