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

TEST_CASE("crc32_64") {
    auto uuid = pfs::from_string<pfs::uuid_t>("01D78XYFJ1PRM1WPBCBT3VHMNV");
    auto crc32 = pfs::crc32(uuid);
    auto crc64 = pfs::crc64(uuid);

    CHECK_EQ(crc32, -1726445405);
    CHECK_EQ(crc64, PFS_INT64_C(531026194733399318));
}
