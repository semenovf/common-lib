////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2021.10.03 Initial version.
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/crc16.hpp"
#include "pfs/crc32.hpp"
#include "pfs/crc64.hpp"

TEST_CASE("crc16_32_64_of") {
    using pfs::crc16_of;
    using pfs::crc16_all_of;
    using pfs::crc32_of;
    using pfs::crc32_all_of;
    using pfs::crc64_of;
    using pfs::crc64_all_of;

    bool v1 = true;
    std::int8_t v2 = 42;
    std::int16_t v3 = 4242;
    std::int32_t v4 = 424242;
    std::int64_t v5 = 42424242LL;

    {
        CHECK_EQ(pfs::crc16_of_ptr("", 0), static_cast<std::int16_t>(0));
        CHECK_EQ(pfs::crc16_of_ptr(" ", 1), static_cast<std::int16_t>(0x2462));
        CHECK_EQ(pfs::crc16_of_ptr("123456789", 9), static_cast<std::int16_t>(0x31C3));

        CHECK_EQ(pfs::crc16_of_ptr("", 0, static_cast<std::int16_t>(0xFFFF)), static_cast<std::int16_t>(0xFFFF));
        CHECK_EQ(pfs::crc16_of_ptr(" ", 1, static_cast<std::int16_t>(0xFFFF)), static_cast<std::int16_t>(0xC592));
        CHECK_EQ(pfs::crc16_of_ptr("123456789", 9, static_cast<std::int16_t>(0xFFFF)), static_cast<std::int16_t>(0x29B1));

        auto a16 = crc16_of(v5
            , crc16_of(v4
                , crc16_of(v3
                    , crc16_of(v2
                        , crc16_of(v1, 0)))));

        auto b16 = crc16_of(v1, 0);
        b16 = crc16_of(v2, b16);
        b16 = crc16_of(v3, b16);
        b16 = crc16_of(v4, b16);
        b16 = crc16_of(v5, b16);

        auto c16 = crc16_all_of(0, v1, v2, v3, v4, v5);

        CHECK_NE(a16, 0);
        CHECK_EQ(a16, b16);
        CHECK_EQ(c16, b16);
    }

    {
        auto a32 = crc32_of(v5
            , crc32_of(v4
                , crc32_of(v3
                    , crc32_of(v2
                        , crc32_of(v1, 0)))));

        auto b32 = crc32_of(v1, 0);
        b32 = crc32_of(v2, b32);
        b32 = crc32_of(v3, b32);
        b32 = crc32_of(v4, b32);
        b32 = crc32_of(v5, b32);

        auto c32 = crc32_all_of(0, v1, v2, v3, v4, v5);

        CHECK_NE(a32, 0);
        CHECK_EQ(a32, b32);
        CHECK_EQ(c32, b32);
    }

    {
        auto a64 = crc64_of(v5
            , crc64_of(v4
                , crc64_of(v3
                    , crc64_of(v2
                        , crc64_of(v1, 0)))));

        auto b64 = crc64_of(v1, 0);
        b64 = crc64_of(v2, b64);
        b64 = crc64_of(v3, b64);
        b64 = crc64_of(v4, b64);
        b64 = crc64_of(v5, b64);

        auto c64 = crc64_all_of(0, v1, v2, v3, v4, v5);

        CHECK_NE(a64, 0);
        CHECK_EQ(a64, b64);
        CHECK_EQ(c64, b64);
    }
}
