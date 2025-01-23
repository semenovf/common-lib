////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2024 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2024.12.23 Initial version.
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define ANKERL_NANOBENCH_IMPLEMENT
#define PFS__UNIVERSAL_ID_IMPL_UUIDV7
#include "doctest.h"
#include "nanobench.h"
#include <pfs/fmt.hpp>
#include <pfs/log.hpp>
#include <pfs/universal_id_uuidv7.hpp>
#include <pfs/universal_id_crc.hpp>
#include <pfs/universal_id_hash.hpp>
#include <unordered_map>

#if defined(QT5_CORE_ENABLED)
#   include <pfs/qt_compat.hpp>
#   include <QUuid>
#endif

TEST_CASE("literal") {
    {
        auto uuid = "01234567-89ab-7000-8000-000000000000"_uuidv7;
        CHECK_EQ(uuid, pfs::from_string<pfs::universal_id>("01234567-89ab-7000-8000-000000000000"));
        CHECK_EQ(to_string(uuid), std::string{"01234567-89ab-7000-8000-000000000000"});
    }
}

TEST_CASE("generate_uuid") {
    auto u1 = pfs::generate_uuid();
    auto u2 = pfs::generate_uuid();
    auto u3 = pfs::generate_uuid();

    LOGD("", "{{{}}}", to_string(u1));
    LOGD("", "{{{}}}", to_string(u2));
    LOGD("", "{{{}}}", to_string(u3));

    REQUIRE_NE(u1, u2);
    REQUIRE_NE(u2, u3);
    REQUIRE_NE(u1, u3);
}

TEST_CASE("crc16_32_64") {
    auto uuid = pfs::from_string<pfs::universal_id>("0193f3a3-c500-717f-9f9c-4740b063c413");
    REQUIRE_NE(uuid, pfs::universal_id{});

    auto crc16 = pfs::crc16_of(uuid);
    auto crc32 = pfs::crc32_of(uuid);
    auto crc64 = pfs::crc64_of(uuid);

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable: 4146) // unary minus operator applied to unsigned type, result still unsigned
#endif

    CHECK_EQ(crc16, -1622);
    CHECK_EQ(crc32, -187475644);
    CHECK_EQ(crc64, PFS_INT64_C(-3441473789579521388));

#if defined(_MSC_VER)
#   pragma warning(pop)
#endif
}

TEST_CASE("serialize")
{
    auto u = pfs::from_string<pfs::universal_id>("0193f3a3-c500-717f-9f9c-4740b063c413");

    REQUIRE_NE(u, pfs::universal_id{});

    auto a = u.to_array();
    CHECK_EQ(a[0], 0x01);
    CHECK_EQ(a[1], 0x93);
    CHECK_EQ(a[2], 0xF3);
    CHECK_EQ(a[3], 0xA3);
    CHECK_EQ(a[4], 0xC5);
    CHECK_EQ(a[5], 0x00);
    CHECK_EQ(a[6], 0x71);
    CHECK_EQ(a[7], 0x7F);
    CHECK_EQ(a[8], 0x9F);
    CHECK_EQ(a[9], 0x9C);
    CHECK_EQ(a[10], 0x47);
    CHECK_EQ(a[11], 0x40);
    CHECK_EQ(a[12], 0xB0);
    CHECK_EQ(a[13], 0x63);
    CHECK_EQ(a[14], 0xC4);
    CHECK_EQ(a[15], 0x13);
}

TEST_CASE("comparison")
{
    auto u1 = pfs::from_string<pfs::universal_id>("{0193f3a3-c500-717f-9f9c-4740b063c413}");
    auto u2 = pfs::from_string<pfs::universal_id>("{0193f3a3-c500-717f-9f9c-4740b063c413}");
    auto u3 = pfs::from_string<pfs::universal_id>("{0193f3a3-c500-724b-84fa-d02d4cae55c0}");
    auto u4 = pfs::from_string<pfs::universal_id>("{0193f3a3-c500-7f24-95be-4d31c0bf06f3}");

    CHECK(u1 == u2);
    CHECK(u1 <= u2);
    CHECK(u1 != u3);
    CHECK(u1  < u3);
    CHECK(u1 <= u3);
    CHECK(u3  > u1);
    CHECK(u3 >= u1);
    CHECK(u4 > u1);
    CHECK(u4 > u2);
    CHECK(u4 > u3);
}

TEST_CASE("hash")
{
    auto u1 = pfs::from_string<pfs::universal_id>("0193f3a3-c500-717f-9f9c-4740b063c413");
    auto u2 = pfs::from_string<pfs::universal_id>("0193f3a3-c500-724b-84fa-d02d4cae55c0");

    std::unordered_map<pfs::universal_id, int> m;
    m[u1] = 42;
    m[u2] = 43;

    CHECK_EQ(m[u1], 42);
    CHECK_EQ(m[u2], 43);
}

#if defined(QT5_CORE_ENABLED)
TEST_CASE("Qt5 QUuid interoperability") {
    for (int i = 0; i < 100; i++) {
        auto u = pfs::generate_uuid();
        QUuid qu = QByteArray::fromStdString(u.to_string());
        CHECK_EQ(u.to_string(), qu.toByteArray(QUuid::WithoutBraces).toStdString());
        //LOGD("-", "{} == {}", u, qu.toByteArray(QUuid::WithBraces).toStdString());
    }
}

TEST_CASE("Compare UUID generation") {
    ankerl::nanobench::Bench().title("UUID").name("UUIDv7").run([] {
        for (int i = 0; i < 1000; i++) {
            auto u = pfs::generate_uuid();
        }
    });

    ankerl::nanobench::Bench().title("UUID").name("QUuid").run([] {
        for (int i = 0; i < 1000; i++) {
            auto u = QUuid::createUuid();
        }
    });
}
#endif
