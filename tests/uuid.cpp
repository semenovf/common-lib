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
#include "pfs/uuid.hpp"
#include "pfs/uuid_crc.hpp"

TEST_CASE("literal") {
    auto uuid = "01D78XYFJ1PRM1WPBCBT3VHMNV"_uuid;
    CHECK_EQ(uuid, pfs::from_string<pfs::uuid_t>("01D78XYFJ1PRM1WPBCBT3VHMNV"));
}

TEST_CASE("crc16_32_64") {
    auto uuid = pfs::from_string<pfs::uuid_t>("01D78XYFJ1PRM1WPBCBT3VHMNV");
    auto crc16 = pfs::crc16_of(uuid);
    auto crc32 = pfs::crc32_of(uuid);
    auto crc64 = pfs::crc64_of(uuid);

    CHECK_EQ(crc16, -20569);
    CHECK_EQ(crc32, -495931481);
    CHECK_EQ(crc64, PFS_INT64_C(-8818850263932298065));
}
