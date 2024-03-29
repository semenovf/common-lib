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

TEST_CASE("sha256") {
    CHECK_EQ(to_string(pfs::crypto::sha256::digest(""))
        , "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");

    CHECK_EQ(to_string(pfs::crypto::sha256::digest("ABCD"))
        , "e12e115acf4552b2568b55e93cbd39394c4ef81c82447fafc997882a02d23677");

    CHECK_EQ(to_string(pfs::crypto::sha256::digest("EFGH"))
        , "e59e4dc24c482ed5ea574fb4f5367f340cb0c77d504bd5b1a982038e2d861954");

    CHECK_EQ(to_string(pfs::crypto::sha256::digest("The quick brown fox jumps over the lazy dog"))
        , "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592");

    CHECK_EQ(to_string(pfs::crypto::sha256::digest("The quick brown fox jumps over the lazy cog"))
        , "e4c4d8f3bf76b692de791a173e05321150f7a345b46484fe427f6acc7ecc81be");
}

TEST_CASE("decode") {
    std::error_code ec;

    CHECK_EQ(pfs::crypto::to_sha256_digest("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855", ec)
        , pfs::crypto::sha256::digest(""));

    CHECK_EQ(pfs::crypto::to_sha256_digest("e12e115acf4552b2568b55e93cbd39394c4ef81c82447fafc997882a02d23677", ec)
        , pfs::crypto::sha256::digest("ABCD"));

    CHECK_EQ(pfs::crypto::to_sha256_digest("e59e4dc24c482ed5ea574fb4f5367f340cb0c77d504bd5b1a982038e2d861954", ec)
        , pfs::crypto::sha256::digest("EFGH"));

    CHECK_EQ(pfs::crypto::to_sha256_digest("d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592", ec)
        , pfs::crypto::sha256::digest("The quick brown fox jumps over the lazy dog"));

    CHECK_EQ(pfs::crypto::to_sha256_digest("e4c4d8f3bf76b692de791a173e05321150f7a345b46484fe427f6acc7ecc81be", ec)
        , pfs::crypto::sha256::digest("The quick brown fox jumps over the lazy cog"));
}
