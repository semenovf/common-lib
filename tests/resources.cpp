////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2024 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2024.07.21 Initial version.
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/resources.hpp"

static std::uint8_t c_data[] = { 0x08, 0x09, 0x0A, 0x0B, 0x0C };
static std::size_t c_length = 5;

#if _MSC_VER
#   define DLL_IMPORT __declspec(dllimport)
#else
#   define DLL_IMPORT
#endif

extern "C" void rc1_init_resources (pfs::resources & instance);
extern "C" DLL_IMPORT void rc2_init_resources (pfs::resources & instance);

extern "C" void rc3_init_resources (pfs::resources & instance)
{
    instance.add("c", c_data, c_length);
};

TEST_CASE("default") {
    pfs::resources res;

    rc1_init_resources(res);
    rc2_init_resources(res);
    rc3_init_resources(res);

    REQUIRE(res.contains("a"));
    REQUIRE(res.contains("b"));
    REQUIRE(res.contains("c"));
    REQUIRE_FALSE(res.contains("d"));

    CHECK_THROWS(res.add("a", c_data, c_length));

    CHECK_EQ(res.get("a")->length, 3);
    CHECK_EQ(res.get("a")->data[0], 0x01);
    CHECK_EQ(res.get("a")->data[1], 0x02);
    CHECK_EQ(res.get("a")->data[2], 0x03);

	CHECK_EQ(res.get("b")->length, 4);
	CHECK_EQ(res.get("b")->data[0], 0x04);
	CHECK_EQ(res.get("b")->data[1], 0x05);
	CHECK_EQ(res.get("b")->data[2], 0x06);
	CHECK_EQ(res.get("b")->data[3], 0x07);

    CHECK_EQ(res.get("c")->length, 5);
    CHECK_EQ(res.get("c")->data[0], 0x08);
    CHECK_EQ(res.get("c")->data[1], 0x09);
    CHECK_EQ(res.get("c")->data[2], 0x0A);
    CHECK_EQ(res.get("c")->data[3], 0x0B);
    CHECK_EQ(res.get("c")->data[4], 0x0C);
}
