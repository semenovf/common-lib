////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2021.12.06 Initial version.
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/sha256.hpp"

using sha256 = pfs::crypto::sha256;

TEST_CASE("sha256") {
    CHECK(to_string(sha256::digest(""))
        == "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");

    CHECK(to_string(sha256::digest("The quick brown fox jumps over the lazy dog"))
        == "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592");

    CHECK(to_string(sha256::digest("The quick brown fox jumps over the lazy cog"))
        == "e4c4d8f3bf76b692de791a173e05321150f7a345b46484fe427f6acc7ecc81be");
}

