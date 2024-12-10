////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2024 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2024.12.10 Initial version.
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/synchronized.hpp"
#include <thread>
#include <vector>

TEST_CASE("basic") {
    pfs::synchronized<std::vector<char>> safe_vector;

    std::thread th1;
    std::thread th2;

    th1 = std::thread([& safe_vector] {
        for (int i = 0; i < 512; i++)
            safe_vector.wlock()->push_back('A');
    });

    th2 = std::thread([& safe_vector] {
        for (int i = 0; i < 512; i++)
            safe_vector.wlock()->push_back('Z');
    });

    th1.join();
    th2.join();

    CHECK_EQ(safe_vector.unsafe().size(), 1024);

    auto a = std::count(safe_vector.unsafe().cbegin(), safe_vector.unsafe().cend(), 'A');
    auto x = std::count(safe_vector.unsafe().cbegin(), safe_vector.unsafe().cend(), 'Z');

    CHECK_EQ(a, x);
}
