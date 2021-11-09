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
#include "pfs/fmt.hpp"
#include "pfs/uuid.hpp"
#include "pfs/uuid_crc.hpp"

TEST_CASE ("") {
#ifdef ULIDUINT128
    fmt::print("*** UUID implemented as `int128` integer\n");
#else
    fmt::print("*** UUID implemented as `struct`\n");
#endif
}

TEST_CASE("literal") {
    auto uuid = "01D78XYFJ1PRM1WPBCBT3VHMNV"_uuid;
    CHECK(uuid == pfs::from_string<pfs::uuid_t>("01D78XYFJ1PRM1WPBCBT3VHMNV"));
}

TEST_CASE("crc16_32_64") {
    auto uuid = pfs::from_string<pfs::uuid_t>("01D78XYFJ1PRM1WPBCBT3VHMNV");
    auto crc16 = pfs::crc16_of(uuid);
    auto crc32 = pfs::crc32_of(uuid);
    auto crc64 = pfs::crc64_of(uuid);

    CHECK_EQ(crc16, 23689);
    CHECK_EQ(crc32, -495931481);
    CHECK_EQ(crc64, PFS_INT64_C(-8818850263932298065));
}

TEST_CASE("serialize")
{
    auto u = pfs::from_string<pfs::uuid_t>("01D78XYFJ1PRM1WPBCBT3VHMNV");

#ifndef ULIDUINT128
    CHECK_EQ(u.data[0], 0x01);
    CHECK_EQ(u.data[1], 0x69);
    CHECK_EQ(u.data[2], 0xD1);
    CHECK_EQ(u.data[3], 0xDF);
    CHECK_EQ(u.data[4], 0x3E);
    CHECK_EQ(u.data[5], 0x41);
    CHECK_EQ(u.data[6], 0xB6);
    CHECK_EQ(u.data[7], 0x28);
    CHECK_EQ(u.data[8], 0x1E);
    CHECK_EQ(u.data[9], 0x59);
    CHECK_EQ(u.data[10], 0x6C);
    CHECK_EQ(u.data[11], 0x5E);
    CHECK_EQ(u.data[12], 0x87);
    CHECK_EQ(u.data[13], 0xB8);
    CHECK_EQ(u.data[14], 0xD2);
    CHECK_EQ(u.data[15], 0xBB);
#endif

    std::array<std::uint8_t, 16> a_little = {
          0xBB, 0xD2, 0xB8, 0x87, 0x5E, 0x6C, 0x59, 0x1E
        , 0x28, 0xB6, 0x41, 0x3E, 0xDF, 0xD1, 0x69, 0x01
    };

    std::array<std::uint8_t, 16> a_big = {
          0x01, 0x69, 0xD1, 0xDF, 0x3E, 0x41, 0xB6, 0x28
        , 0x1E, 0x59, 0x6C, 0x5E, 0x87, 0xB8, 0xD2, 0xBB
    };

    auto u1 = pfs::make_uuid(a_little, pfs::endian::little);
    auto u2 = pfs::make_uuid(a_big, pfs::endian::big);

    CHECK(u == u1);
    CHECK(u == u2);

    auto a1 = pfs::to_array(u1, pfs::endian::little);
    auto a2 = pfs::to_array(u2, pfs::endian::big);

    for (int i = 0; i < 16; i++)
        CHECK_EQ(a1[i], a_little[i]);

    for (int i = 0; i < 16; i++)
        CHECK_EQ(a2[i], a_big[i]);
}
