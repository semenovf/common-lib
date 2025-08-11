////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2025.08.06 Initial version.
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/filesystem_pack.hpp"
#include "pfs/string_view.hpp"
#include "pfs/universal_id_pack.hpp"
#include "pfs/v2/binary_ostream.hpp"
#include <cstring>
#include <limits>
#include <string>
#include <vector>

static std::string s_sample_data_le (
    "\x01\x61\xD6\x2A\x00\x80\xFF\xFF"
    "\x00\x00\x00\x80"
    "\xFF\xFF\xFF\xFF"
    "\x00\x00\x00\x00\x00\x00\x00\x80"
    "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
    "\xD0\x0F\x49\x40"
    "\x90\xF7\xAA\x95\x09\xBF\x05\x40"
    "Hello"
    ",World!"
    "Bye everyone!"
    "And hello again!"
    "vector"
    "array"
    "\x2A\x00"
    "x"
    , 99);

static std::string s_sample_data_be (
    "\x01\x61\xD6\x2A\x80\x00\xFF\xFF"
    "\x80\x00\x00\x00"
    "\xFF\xFF\xFF\xFF"
    "\x80\x00\x00\x00\x00\x00\x00\x00"
    "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
    "\x40\x49\x0F\xD0"
    "\x40\x05\xBF\x09\x95\xAA\xF7\x90"
    "Hello"
    ",World!"
    "Bye everyone!"
    "And hello again!"
    "vector"
    "array"
    "\x00\x2A"
    "x"
    , 99);

enum class test_enum: std::int16_t { test = 42 };

struct A
{
    char ch;
};

template <pfs::endian Endianess, typename Archive>
inline void pack (pfs::v2::binary_ostream<Endianess, Archive> & out, A const & a)
{
    out << a.ch;
}

template <pfs::endian Endianess, typename Archive>
void serialize ()
{
    using binary_ostream_t = pfs::v2::binary_ostream<Endianess, Archive>;

    Archive ar;
    binary_ostream_t out {ar};

    out << true
        << 'a'
        << std::int8_t{-42}
        << std::uint8_t{42}
        << (std::numeric_limits<std::int16_t>::min)()
        << (std::numeric_limits<std::uint16_t>::max)()
        << (std::numeric_limits<std::int32_t>::min)()
        << (std::numeric_limits<std::uint32_t>::max)()
        << (std::numeric_limits<std::int64_t>::min)()
        << (std::numeric_limits<std::uint64_t>::max)()
        << float{3.14159}
        << double{2.71828}
        << "Hello"
        << std::string(",World!")
        << pfs::string_view("Bye everyone!")
        << std::make_pair("And hello again!", 16)
        << std::vector<char>{'v', 'e', 'c', 't', 'o', 'r'}
        << std::array<char, 5>{'a', 'r', 'r', 'a', 'y'}
        << test_enum::test
        << A{'x'};

    if (Endianess == pfs::endian::little) {
        REQUIRE_EQ(s_sample_data_le.size(), ar.size());
        CHECK_EQ(std::memcmp(s_sample_data_le.data(), ar.data(), ar.size()), 0);
    } else {
        REQUIRE_EQ(s_sample_data_be.size(), ar.size());
        CHECK_EQ(std::memcmp(s_sample_data_be.data(), ar.data(), ar.size()), 0);
    }
}

template <pfs::endian Endianess, typename Archive>
void serialize_universal_id ()
{
    using binary_ostream_t = pfs::v2::binary_ostream<Endianess, Archive>;

    Archive ar;
    binary_ostream_t out {ar};

#if defined(PFS__UNIVERSAL_ID_IMPL_UUIDV7)
    auto uuid = "0193f3a3-c500-717f-9f9c-4740b063c413"_uuidv7;

    out << uuid;

    if (Endianess == pfs::endian::big) {
        CHECK_EQ(std::memcmp("\x01\x93\xF3\xA3\xC5\x00\x71\x7F\x9F\x9C\x47\x40\xB0\x63\xC4\x13"
            , ar.data(), ar.size()), 0);
    } else {
        CHECK_EQ(std::memcmp("\x13\xC4\x63\xB0\x40\x47\x9C\x9F\x7F\x71\x00\xC5\xA3\xF3\x93\x01"
            , ar.data(), ar.size()), 0);
    }
#else
    auto uuid = "01D78XYFJ1PRM1WPBCBT3VHMNV"_uuid;

    out << uuid;

    if (Endianess == pfs::endian::big) {
        CHECK_EQ(std::memcmp("\x01\x69\xD1\xDF\x3E\x41\xB6\x28\x1E\x59\x6C\x5E\x87\xB8\xD2\xBB"
            , ar.data(), ar.size()), 0);
    } else {
        CHECK_EQ(std::memcmp("\xBB\xD2\xB8\x87\x5E\x6C\x59\x1E\x28\xB6\x41\x3E\xDF\xD1\x69\x01"
            , ar.data(), ar.size()), 0);
    }
#endif
}

template <pfs::endian Endianess, typename Archive>
void serialize_filesystem_path ()
{
    using binary_ostream_t = pfs::v2::binary_ostream<Endianess, Archive>;

    Archive ar;
    binary_ostream_t out {ar};

    pfs::filesystem::path path = PFS__LITERAL_PATH("/path/to/some/file.ext");
    out << path;

    if (Endianess == pfs::endian::big) {
        CHECK_EQ(std::memcmp("\x00\x16/path/to/some/file.ext", ar.data(), ar.size()), 0);
    } else {
        CHECK_EQ(std::memcmp("\x16\x00/path/to/some/file.ext", ar.data(), ar.size()), 0);
    }
}

TEST_CASE("basic serialization") {
    serialize<pfs::endian::big, std::vector<char>>();
    serialize<pfs::endian::little, std::vector<char>>();
}

TEST_CASE("Universal ID serialization") {
    serialize_universal_id<pfs::endian::big, std::vector<char>>();
    serialize_universal_id<pfs::endian::little, std::vector<char>>();
}

TEST_CASE("Filesystem path serialization") {
    serialize_filesystem_path<pfs::endian::big, std::vector<char>>();
    serialize_filesystem_path<pfs::endian::little, std::vector<char>>();
}
