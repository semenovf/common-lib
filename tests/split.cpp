////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2021.07.06 Initial version.
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/split.hpp"
#include <string>
#include <iterator>
#include <vector>

TEST_CASE("split") {
    using pfs::split;

    {
        std::string source{"?;C?;CP?;CR?"};
        std::string separator{";"};
        std::vector<std::string> result;

        split(std::back_inserter(result)
            , source.begin(), source.end()
            , separator.begin(), separator.end()
            , pfs::keep_empty::yes);

        CHECK(result[0] == "?");
        CHECK(result[1] == "C?");
        CHECK(result[2] == "CP?");
        CHECK(result[3] == "CR?");
    }

    {
        std::string source{"A//B/C/"};
        std::string separator{"/"};
        std::vector<std::string> result;

        split(std::back_inserter(result)
            , source.begin(), source.end()
            , separator.begin(), separator.end()
            , pfs::keep_empty::yes);

        CHECK(result[0] == "A");
        CHECK(result[1] == "");
        CHECK(result[2] == "B");
        CHECK(result[3] == "C");
        CHECK(result[4] == "");
    }

    {
        std::string source{"A//B/C/"};
        std::string separator{"/"};
        std::vector<std::string> result;

        split(std::back_inserter(result)
            , source.begin(), source.end()
            , separator.begin(), separator.end()
            , pfs::keep_empty::no);

        CHECK(result[0] == "A");
        CHECK(result[1] == "B");
        CHECK(result[2] == "C");
    }
}

