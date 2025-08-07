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
#include "pfs/v2/binary_ostream.hpp"
#include "pfs/string_view.hpp"
#include <cstring>
#include <limits>
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

template <typename Archive, pfs::endian Endianess>
inline pfs::v2::binary_ostream<Archive, Endianess> &
operator << (pfs::v2::binary_ostream<Archive, Endianess> & out, A const & a)
{
    out << a.ch;
    return out;
}

template <typename Archive, pfs::endian Endianess>
void serialize ()
{
    using binary_ostream_t = pfs::v2::binary_ostream<Archive, Endianess>;

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

TEST_CASE("little endian order") {
    serialize<std::vector<char>, pfs::endian::little>();
}

TEST_CASE("big endian order") {
    serialize<std::vector<char>, pfs::endian::big>();
}
