////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2025.08.07 Initial version.
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/filesystem_pack.hpp"
#include "pfs/string_view.hpp"
#include "pfs/universal_id_pack.hpp"
#include "pfs/v2/binary_istream.hpp"
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
    "vector"
    "array"
    "\x2A\x00"
    "x"
    , 83);

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
    "vector"
    "array"
    "\x00\x2A"
    "x"
    , 83);

enum class test_enum: std::int16_t { test = 42 };

struct A
{
    char ch;
};

template <pfs::endian Endianess>
inline void unpack (pfs::v2::binary_istream<Endianess> & in, A & a)
{
    in >> a.ch;
}

template <pfs::endian Endianess>
void deserialize (std::string const & sample_string)
{
    using binary_istream_t = pfs::v2::binary_istream<Endianess>;

    binary_istream_t in {sample_string.data(), sample_string.size()};

    bool b = false;
    in >> b;
    CHECK_EQ(b, true);

    char c = '\x00';
    in >> c;
    CHECK_EQ(c, 'a');

    std::int8_t i8 = 0;
    in >> i8;
    CHECK_EQ(i8, std::int8_t{-42});

    std::uint8_t u8 = 0;
    in >> u8;
    CHECK_EQ(u8, std::uint8_t{42});

    std::int16_t i16 = 0;
    in >> i16;
    CHECK_EQ(i16, (std::numeric_limits<std::int16_t>::min)());

    std::uint16_t u16 = 0;
    in >> u16;
    CHECK_EQ(u16, (std::numeric_limits<std::uint16_t>::max)());

    std::int32_t i32 = 0;
    in >> i32;
    CHECK_EQ(i32, (std::numeric_limits<std::int32_t>::min)());

    std::uint32_t u32 = 0;
    in >> u32;
    CHECK_EQ(u32, (std::numeric_limits<std::uint32_t>::max)());

    std::int64_t i64 = 0;
    in >> i64;
    CHECK_EQ(i64, (std::numeric_limits<std::int64_t>::min)());

    std::uint64_t u64 = 0;
    in >> u64;
    CHECK_EQ(u64, (std::numeric_limits<std::uint64_t>::max)());

    float f32 = 0;
    in >> f32;
    CHECK_EQ(f32, float{3.14159});

    double f64 = 0;
    in >> f64;
    CHECK_EQ(f64, double{2.71828});

    char hello[5];
    in >> std::make_pair(hello, sizeof(hello));
    CHECK_EQ(std::memcmp(hello, "Hello", sizeof(hello)), 0);

    std::string world;
    in >> std::make_pair(& world, 7);
    CHECK_EQ(world, std::string(",World!"));

    pfs::string_view bye;
    in >> std::make_pair(& bye, 13);
    CHECK_EQ(bye, pfs::string_view("Bye everyone!"));

    std::vector<char> vec;
    in >> std::make_pair(& vec, 6);
    CHECK_EQ(vec, std::vector<char>{'v', 'e', 'c', 't', 'o', 'r'});

    std::array<char, 5> arr;
    in >> arr;
    CHECK_EQ(arr, std::array<char, 5>{'a', 'r', 'r', 'a', 'y'});

    test_enum en;
    in >> en;
    CHECK_EQ(en, test_enum::test);

    A a {'\x00'};
    in >> a;
    CHECK_EQ(a.ch, 'x');
}

template <pfs::endian Endianess>
void deserialize_universal_id ()
{
    using binary_istream_t = pfs::v2::binary_istream<Endianess>;

    pfs::universal_id uuid;

#if defined(PFS__UNIVERSAL_ID_IMPL_UUIDV7)
    auto uuid_sample = "0193f3a3-c500-717f-9f9c-4740b063c413"_uuidv7;
    std::string source;

    if (Endianess == pfs::endian::big) {
        source = std::string("\x01\x93\xF3\xA3\xC5\x00\x71\x7F\x9F\x9C\x47\x40\xB0\x63\xC4\x13", 16);
    } else {
        source = std::string("\x13\xC4\x63\xB0\x40\x47\x9C\x9F\x7F\x71\x00\xC5\xA3\xF3\x93\x01", 16);
    }

    binary_istream_t in {source.data(), source.size()};
    in >> uuid;
#else
    auto uuid_sample = "01D78XYFJ1PRM1WPBCBT3VHMNV"_uuid;
    std::string source;

    if (Endianess == pfs::endian::big) {
        source = std::string("\x01\x69\xD1\xDF\x3E\x41\xB6\x28\x1E\x59\x6C\x5E\x87\xB8\xD2\xBB", 16);
    } else {
        source = std::string("\xBB\xD2\xB8\x87\x5E\x6C\x59\x1E\x28\xB6\x41\x3E\xDF\xD1\x69\x01", 16);
    }

    binary_istream_t in {source.data(), source.size()};
    in >> uuid;
#endif

    CHECK_EQ(uuid, uuid_sample);
}

template <pfs::endian Endianess>
void deserialize_filesystem_path ()
{
    using binary_istream_t = pfs::v2::binary_istream<Endianess>;

    pfs::filesystem::path path_sample = PFS__LITERAL_PATH("/path/to/some/file.ext");

    std::string source;

    if (Endianess == pfs::endian::big) {
        source = std::string("\x00\x16/path/to/some/file.ext", 24);
    } else {
        source = std::string("\x16\x00/path/to/some/file.ext", 24);
    }

    pfs::filesystem::path path;
    binary_istream_t in {source.data(), source.size()};
    in >> path;

    CHECK_EQ(path, path_sample);
}

TEST_CASE("basic deserialization") {
    deserialize<pfs::endian::big>(s_sample_data_be);
    deserialize<pfs::endian::little>(s_sample_data_le);
}

TEST_CASE("Universal ID deserialization") {
    deserialize_universal_id<pfs::endian::big>();
    deserialize_universal_id<pfs::endian::little>();
}

TEST_CASE("Filesystem path deserialization") {
    deserialize_filesystem_path<pfs::endian::big>();
    deserialize_filesystem_path<pfs::endian::little>();
}
