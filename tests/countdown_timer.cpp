////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2025.01.23 Initial version.
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <pfs/countdown_timer.hpp>
#include <thread>

TEST_CASE("countdown_timer") {
    pfs::countdown_timer<std::milli> cdt {std::chrono::milliseconds{100}};
    std::this_thread::sleep_for(std::chrono::milliseconds{90});

    CHECK(cdt.remain_count() > 0);

    cdt.reset();
    std::this_thread::sleep_for(std::chrono::milliseconds{110});

    CHECK(cdt.remain_count() == 0);
}
