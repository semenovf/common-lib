////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of [common-lib](https://github.com/semenovf/common-lib) library.
//
// Changelog:
//      2021.07.06 Initial version.
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/find.hpp"
#include <string>

TEST_CASE("find") {
    using pfs::find;

    std::string haystack{"Hello, World!"};

    {
        std::string needle{"H"};
        auto r = find(haystack.begin()
            , haystack.end()
            , needle.begin()
            , needle.end());

        CHECK_EQ(r.first, haystack.begin());
        CHECK_EQ(r.second, haystack.begin() + 1);
    }

    {
        std::string needle{"ello"};
        auto r = find(haystack.begin()
            , haystack.end()
            , needle.begin()
            , needle.end());

        CHECK_EQ(r.first, haystack.begin() + 1);
        CHECK_EQ(r.second, haystack.begin() + 5);
    }

    {
        std::string needle{"!"};
        auto r = find(haystack.begin()
            , haystack.end()
            , needle.begin()
            , needle.end());

        CHECK_EQ(r.first, haystack.begin() + haystack.size() - 1);
        CHECK_EQ(r.second, haystack.end());
    }

    {
        std::string needle{"?"};
        auto r = find(haystack.begin()
            , haystack.end()
            , needle.begin()
            , needle.end());

        CHECK_EQ(r.first, haystack.end());
        CHECK_EQ(r.second, haystack.end());
    }

}
