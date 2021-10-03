////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of [common-lib](https://github.com/semenovf/common-lib) library.
//
// Changelog:
//      2021.10.03 Initial version.
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/crc32.hpp"
#include "pfs/crc64.hpp"

TEST_CASE("crc32_64_of") {
    bool v1 = true;
    std::int8_t v2 = 42;
    std::int16_t v3 = 4242;
    std::int32_t v4 = 424242;
    std::int64_t v5 = 42424242LL;

    {
        auto a32 = pfs::crc32(v5
            , pfs::crc32(v4
                , pfs::crc32(v3
                    , pfs::crc32(v2
                        , pfs::crc32(v1, 0)))));

        auto b32 = pfs::crc32(v1, 0);
        b32 = pfs::crc32(v2, b32);
        b32 = pfs::crc32(v3, b32);
        b32 = pfs::crc32(v4, b32);
        b32 = pfs::crc32(v5, b32);

        auto c32 = pfs::crc32_of(0, v1, v2, v3, v4, v5);

        CHECK_NE(a32, 0);
        CHECK_EQ(a32, b32);
        CHECK_EQ(c32, b32);
    }

    {
        auto a64 = pfs::crc64(v5
            , pfs::crc64(v4
                , pfs::crc64(v3
                    , pfs::crc64(v2
                        , pfs::crc64(v1, 0)))));

        auto b64 = pfs::crc64(v1, 0);
        b64 = pfs::crc64(v2, b64);
        b64 = pfs::crc64(v3, b64);
        b64 = pfs::crc64(v4, b64);
        b64 = pfs::crc64(v5, b64);

        auto c64 = pfs::crc64_of(0, v1, v2, v3, v4, v5);

        CHECK_NE(a64, 0);
        CHECK_EQ(a64, b64);
        CHECK_EQ(c64, b64);
    }
}
