////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2023-2025 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2023.03.22 Initial version.
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/binary_istream.hpp"
#include "pfs/binary_ostream.hpp"
#include <vector>

template <pfs::endian Endianess>
void serialize ()
{
    std::vector<char> buffer;
    pfs::binary_ostream<Endianess> os {buffer};

    char a0          = 'a';
    std::int8_t b0   = std::int8_t{-42};
    std::uint8_t c0  = std::uint8_t{42};
    std::int16_t d0  = (std::numeric_limits<std::int16_t>::min)();
    std::uint16_t e0 = (std::numeric_limits<std::uint16_t>::max)();
    std::int32_t f0  = (std::numeric_limits<std::int32_t>::min)();
    std::uint32_t g0 = (std::numeric_limits<std::uint32_t>::max)();
    std::int64_t h0  = (std::numeric_limits<std::int64_t>::min)();
    std::uint64_t i0 = (std::numeric_limits<std::uint64_t>::max)();
    float j0         = 3.14159f;
    double k0        = 3.14159f;
    std::string s0   = "Hello";

    os << a0 << b0 << c0 << d0 << e0 << f0 << g0 << h0 << i0 << j0 << k0
        << s0.size() << std::make_pair(s0.data(), s0.size()) << a0;

    pfs::binary_istream<Endianess> is {buffer.data(), buffer.size()};

    char a1, a2, a3;
    std::int8_t b1;
    std::uint8_t c1;
    std::int16_t d1;
    std::uint16_t e1;
    std::int32_t f1;
    std::uint32_t g1;
    std::int64_t h1;
    std::uint64_t i1;
    float j1;
    double k1;
    std::string s1;
    std::string::size_type sz1;

    is >> a1 >> b1 >> c1 >> d1 >> e1 >> f1 >> g1 >> h1 >> i1 >> j1 >> k1
        >> sz1 >> std::make_pair(& s1, & sz1) >> a2;

    CHECK_EQ(a1, a0);
    CHECK_EQ(b1, b0);
    CHECK_EQ(c1, c0);
    CHECK_EQ(d1, d0);
    CHECK_EQ(e1, e0);
    CHECK_EQ(f1, f0);
    CHECK_EQ(g1, g0);
    CHECK_EQ(h1, h0);
    CHECK_EQ(i1, i0);
    CHECK_EQ(j1, j0);
    CHECK_EQ(k1, k0);
    CHECK_EQ(s1, s0);

    is.start_transaction();
    is >> a3;

    CHECK_FALSE(is.commit_transaction());
}

TEST_CASE("native order") { serialize<pfs::endian::native>(); }
TEST_CASE("network order") { serialize<pfs::endian::network>(); }
