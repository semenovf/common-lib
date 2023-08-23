////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2021.10.13 Initial version.
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define ANKERL_NANOBENCH_IMPLEMENT
#include "doctest.h"
#include "nanobench.h"
#include "pfs/byteswap.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>

#include "pfs/fmt.hpp"

TEST_CASE("byteswap") {
    CHECK_EQ(pfs::byteswap(std::uint16_t{0xCAFE}), std::uint16_t{0xFECA});
    CHECK_EQ(pfs::byteswap(std::uint32_t{0xDEADBEEFu}), std::uint32_t{0xEFBEADDEu});
    CHECK_EQ(pfs::byteswap(std::uint64_t{0x0123456789ABCDEFull})
        , std::uint64_t{0xEFCDAB8967452301ull});

    CHECK_EQ(pfs::byteswap(pfs::byteswap(std::uint16_t{0xCAFE})), std::uint16_t{0xCAFE});
    CHECK_EQ(pfs::byteswap(pfs::byteswap(std::uint32_t{0xDEADBEEFu})), std::uint32_t{0xDEADBEEFu});
    CHECK_EQ(pfs::byteswap(pfs::byteswap(std::uint64_t{0x0123456789ABCDEFull}))
        , std::uint64_t{0x0123456789ABCDEFull});

#if defined(PFS_HAS_INT128)
    {
        auto x = pfs::construct_uint128(0x1234567890ABCDEFull, 0x1234567890ABCDEFull);
        auto y = pfs::construct_uint128(0xEFCDAB9078563412ull, 0xEFCDAB9078563412ull);

        auto z1 = pfs::byteswap(x);
        auto z2 = pfs::byteswap(pfs::byteswap(x));

        CHECK_EQ(z1, y);
        CHECK_EQ(z2, x);

        int128_type a = -1;

        CHECK_EQ(pfs::byteswap(pfs::byteswap(a)), a);
    }

    {
        union u128 {
            uint128_type x;
            std::uint8_t b[sizeof(int128_type)];
        };

        u128 a;
        a.x = pfs::construct_uint128(0x1234567890ABCDEFull, 0xAB12CD34EF56BA78ull);

        u128 b;
        b.x = pfs::byteswap(a.x);

        fmt::print("a.x = {:X}\n", a.x);
        fmt::print("a.b = {:X}{:X}{:X}{:X}{:X}{:X}{:X}{:X}"
            "{:X}{:X}{:X}{:X}{:X}{:X}{:X}{:X}\n"
            , a.b[0], a.b[1], a.b[2], a.b[3], a.b[4], a.b[5], a.b[6], a.b[7]
            , a.b[8], a.b[9], a.b[10], a.b[11], a.b[12], a.b[13], a.b[14], a.b[15]);

        fmt::print("b.x = {:X}\n", b.x);
        fmt::print("b.b = {:X}{:X}{:X}{:X}{:X}{:X}{:X}{:X}"
            "{:X}{:X}{:X}{:X}{:X}{:X}{:X}{:X}\n"
            , b.b[0], b.b[1], b.b[2], b.b[3], b.b[4], b.b[5], b.b[6], b.b[7]
            , b.b[8], b.b[9], b.b[10], b.b[11], b.b[12], b.b[13], b.b[14], b.b[15]);

        CHECK_EQ(a.b[0] , 0x78);
        CHECK_EQ(a.b[1] , 0xBA);
        CHECK_EQ(a.b[2] , 0x56);
        CHECK_EQ(a.b[3] , 0xEF);
        CHECK_EQ(a.b[4] , 0x34);
        CHECK_EQ(a.b[5] , 0xCD);
        CHECK_EQ(a.b[6] , 0x12);
        CHECK_EQ(a.b[7] , 0xAB);
        CHECK_EQ(a.b[8] , 0xEF);
        CHECK_EQ(a.b[9] , 0xCD);
        CHECK_EQ(a.b[10], 0xAB);
        CHECK_EQ(a.b[11], 0x90);
        CHECK_EQ(a.b[12], 0x78);
        CHECK_EQ(a.b[13], 0x56);
        CHECK_EQ(a.b[14], 0x34);
        CHECK_EQ(a.b[15], 0x12);

        CHECK_EQ(b.b[0] , 0x12);
        CHECK_EQ(b.b[1] , 0x34);
        CHECK_EQ(b.b[2] , 0x56);
        CHECK_EQ(b.b[3] , 0x78);
        CHECK_EQ(b.b[4] , 0x90);
        CHECK_EQ(b.b[5] , 0xAB);
        CHECK_EQ(b.b[6] , 0xCD);
        CHECK_EQ(b.b[7] , 0xEF);
        CHECK_EQ(b.b[8] , 0xAB);
        CHECK_EQ(b.b[9] , 0x12);
        CHECK_EQ(b.b[10], 0xCD);
        CHECK_EQ(b.b[11], 0x34);
        CHECK_EQ(b.b[12], 0xEF);
        CHECK_EQ(b.b[13], 0x56);
        CHECK_EQ(b.b[14], 0xBA);
        CHECK_EQ(b.b[15], 0x78);

        CHECK_EQ(pfs::byteswap(pfs::byteswap(a.x)), a.x);
    }

#endif
}

#if defined(PFS_HAS_INT128)
inline constexpr __uint128_t construct_uint128 (std::uint64_t hi
    , std::uint64_t low) noexcept
{
    return (static_cast<__uint128_t>(hi) << 64) | low;
}

inline constexpr __uint128_t construct_uint128 (std::uint32_t a
    , std::uint32_t b
    , std::uint32_t c
    , std::uint32_t d) noexcept
{
    return (static_cast<__uint128_t>(a) << 96)
        | (static_cast<__uint128_t>(b) << 64)
        | (static_cast<__uint128_t>(c) << 32)
        | d;
}

TEST_CASE("construct_uint128")
{
    CHECK_EQ(construct_uint128(0,0), __uint128_t(0));
    CHECK_EQ(construct_uint128(0,1), __uint128_t(1));
    CHECK_EQ(construct_uint128(0xFFFFFFFFFFFFFFFFull
        , 0xFFFFFFFFFFFFFFFFull), __uint128_t(-1));
    CHECK_EQ(construct_uint128(0xFFFFFFFFul
        , 0xFFFFFFFFul
        , 0xFFFFFFFFul
        , 0xFFFFFFFFul), __uint128_t(-1));

//     fmt::print("-- '{:X}'\n", __uint128_t(-1));
//     fmt::print("-- '{:X}'\n", __uint128_t(-1) - __uint128_t(-1)/128);
//     fmt::print("-- '{:X}'\n", __uint128_t(-1) - __uint128_t(-1)/128);
//     fmt::print("-- '{:X}'\n", construct_uint128(0x1234567890ABCDEFull, 0x1234567890ABCDEFull));
}
#endif

namespace classic {

template <typename T>
PFS_BYTESWAP_CONSTEXPR T byteswap (T n) noexcept;

template <>
inline PFS_BYTESWAP_CONSTEXPR
std::uint8_t byteswap<std::uint8_t> (std::uint8_t x) noexcept
{
    return x;
}

template <>
inline PFS_BYTESWAP_CONSTEXPR
std::uint16_t byteswap<std::uint16_t> (std::uint16_t x) noexcept
{
    return (x >> 8) | (x << 8);;
}

template <>
inline PFS_BYTESWAP_CONSTEXPR
std::uint32_t byteswap<std::uint32_t> (std::uint32_t x) noexcept
{
    std::uint32_t y = ((x << 8) & 0xFF00FF00) | ((x >> 8) & 0xFF00FF);
    return (y << 16) | (y >> 16);
}

template <>
inline PFS_BYTESWAP_CONSTEXPR
std::uint64_t byteswap<std::uint64_t> (std::uint64_t x) noexcept
{
    x = ((x & 0x00000000FFFFFFFFull) << 32) | ((x & 0xFFFFFFFF00000000ull) >> 32);
    x = ((x & 0x0000FFFF0000FFFFull) << 16) | ((x & 0xFFFF0000FFFF0000ull) >> 16);
    x = ((x & 0x00FF00FF00FF00FFull) << 8)  | ((x & 0xFF00FF00FF00FF00ull) >> 8);
    return x;
}

#if defined(PFS_HAS_INT128)
template <>
inline PFS_BYTESWAP_CONSTEXPR
__uint128_t byteswap<__uint128_t> (__uint128_t x) noexcept
{
    return construct_uint128(byteswap(static_cast<std::uint64_t>(x >> 64))
        , byteswap(static_cast<std::uint64_t>(x)));
}
#endif

template <typename T>
struct byteswap_helper
{
    PFS_BYTESWAP_CONSTEXPR T operator () (T x) const noexcept
    {
        return byteswap<T>(x);
    }
};

} // namespace classic

namespace intrinsics {

template <typename T>
PFS_BYTESWAP_CONSTEXPR T byteswap (T n) noexcept;

template <>
inline PFS_BYTESWAP_CONSTEXPR
std::uint8_t byteswap<std::uint8_t> (std::uint8_t x) noexcept
{
    return x;
}

#if PFS__COMPILER_GCC

#   define PFS_BYTESWAP_HAS_INTRINSICS 1

template <>
inline PFS_BYTESWAP_CONSTEXPR
std::uint16_t byteswap<std::uint16_t> (std::uint16_t x) noexcept
{
    return __builtin_bswap16(x);
}

template <>
inline PFS_BYTESWAP_CONSTEXPR
std::uint32_t byteswap<std::uint32_t> (std::uint32_t x) noexcept
{
    return __builtin_bswap32(x);
}

template <>
inline PFS_BYTESWAP_CONSTEXPR
std::uint64_t byteswap<std::uint64_t> (std::uint64_t x) noexcept
{
    return __builtin_bswap64(x);
}

#elif PFS__COMPILER_MSVC
#   define PFS_BYTESWAP_HAS_INTRINSICS 1

// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference
//      /byteswap-uint64-byteswap-ulong-byteswap-ushort?view=msvc-160

template <>
inline PFS_BYTESWAP_CONSTEXPR
std::uint16_t byteswap<std::uint16_t> (std::uint16_t x) noexcept
{
    return _byteswap_ushort(x);
}

template <>
inline PFS_BYTESWAP_CONSTEXPR
std::uint32_t byteswap<std::uint32_t> (std::uint32_t x) noexcept
{
    return _byteswap_ulong(x);
}

template <>
inline PFS_BYTESWAP_CONSTEXPR
std::uint64_t byteswap<std::uint64_t> (std::uint64_t x) noexcept
{
    return _byteswap_uint64(x);
}

#endif

#if defined(PFS_HAS_INT128)
template <>
inline PFS_BYTESWAP_CONSTEXPR
__uint128_t byteswap<__uint128_t> (__uint128_t x) noexcept
{
    return construct_uint128(byteswap(static_cast<std::uint64_t>(x >> 64))
        , byteswap(static_cast<std::uint64_t>(x)));
}
#endif

template <typename T>
struct byteswap_helper
{
    PFS_BYTESWAP_CONSTEXPR T operator () (T x) const noexcept
    {
        return byteswap<T>(x);
    }
};

} // namespace intrinsics

// https://mklimenko.github.io/english/2018/08/22/robust-endian-swap/
namespace modern {

template <typename T>
T byteswap (T x, typename std::enable_if<std::is_arithmetic<T>::value
#if defined(PFS_HAS_INT128)
        || std::is_same<T, __int128>::value
        || std::is_same<T, unsigned __int128>::value
#endif
        , std::nullptr_t>::type = nullptr)
{
    union U {
        T x;
        std::array<std::uint8_t, sizeof(T)> raw;
    } src, dst;

    src.x = x;
    std::reverse_copy(src.raw.begin(), src.raw.end(), dst.raw.begin());
    return dst.x;
}

template <typename T>
struct byteswap_helper
{
    PFS_BYTESWAP_CONSTEXPR T operator () (T x) const noexcept
    {
        return byteswap<T>(x);
    }
};

} // modern

template <typename Integral, template <typename> class ByteSwapHelper>
void benchmark_op ()
{
    ByteSwapHelper<Integral> byteswap;

    for (Integral i = static_cast<Integral>(-1) - 10000, count = 10000; count; i++, --count) {
        assert(i == byteswap(byteswap(i)));
    }
}

#if defined(PFS_HAS_INT128)
TEST_CASE("byteswap_uint128") {
    auto x = construct_uint128(0x1234567890ABCDEFull, 0x1234567890ABCDEFull);
    auto y = construct_uint128(0xEFCDAB9078563412ull, 0xEFCDAB9078563412ull);

    auto y1 = modern::byteswap(x);
    auto y2 = intrinsics::byteswap(x);

    CHECK_EQ(y, y1);
    CHECK_EQ(y, y2);
}
#endif

//       |      ns/op |      op/s |    err% |     total | std::uint16_t
//       |-----------:|----------:|--------:|----------:|:--------------
// best  |  63,680.00 | 15,703.52 |    0.1% |      0.00 | `classic`
//       |  64,811.00 | 15,429.48 |    0.7% |      0.00 | `intrinsics`
// worst | 464,147.00 |  2,154.49 |    2.1% |      0.01 | `modern`
//
//       |      ns/op |      op/s |    err% |     total | std::uint32_t
//       |-----------:|----------:|--------:|----------:|:--------------
//       |  68,723.00 | 14,551.17 |    0.1% |      0.00 | `classic`
// best  |  62,432.00 | 16,017.43 |    0.1% |      0.00 | `intrinsics`
// worst | 518,626.00 |  1,928.17 |    0.0% |      0.01 | `modern`
//
//       |      ns/op |      op/s |    err% |     total | std::uint64_t
//       |-----------:|----------:|--------:|----------:|:--------------
//       |  91,605.00 | 10,916.43 |    0.1% |      0.00 | `classic`
// best  |  69,673.00 | 14,352.76 |    0.1% |      0.00 | `intrinsics`
// worst | 660,905.00 |  1,513.08 |    2.2% |      0.01 | `modern`

//==============================================================================
// Ubuntu 20.04.3 LTS
// Intel(R) Core(TM) i5-2500K CPU @ 3.30GHz
// RAM 16 Gb
//==============================================================================
//       |        ns/op |     op/s |    err% |     total | std::uint16_t
//       |-------------:|---------:|--------:|----------:|:--------------
//       |   258,033.00 | 3,875.47 |    0.1% |      0.00 | `classic`
// worst   818,143.00 | 1,222.28 |    0.5% |      0.01 | `modern`
// best  |   131,936.00 | 7,579.43 |    0.1% |      0.00 | `intrinsics`
//
//       |        ns/op |     op/s |    err% |     total | std::uint32_t
//       |-------------:|---------:|--------:|----------:|:--------------
//       |   152,698.00 | 6,548.87 |    0.0% |      0.00 | `classic`
// worst |   780,683.00 | 1,280.93 |    3.3% |      0.01 | `modern`
// best  |   109,393.00 | 9,141.35 |    4.2% |      0.00 | `intrinsics`
//
//       |        ns/op |     op/s |    err% |     total | std::uint64_t
//       |-------------:|---------:|--------:|----------:|:--------------
//       |   184,616.00 | 5,416.65 |    0.0% |      0.00 | `classic`
// worst |   945,372.00 | 1,057.78 |    0.9% |      0.01 | `modern`
// best  |   108,610.00 | 9,207.26 |    0.0% |      0.00 | `intrinsics`
//
//       |        ns/op |     op/s |    err% |     total | __uint128_t
//       |-------------:|---------:|--------:|----------:|:------------
//       |   463,018.00 | 2,159.74 |    0.1% |      0.01 | `classic`
// worst | 1,317,091.00 |   759.25 |    3.5% |      0.01 | `modern`
// best  |   286,796.00 | 3,486.80 |    1.5% |      0.00 | `intrinsics`

TEST_CASE("benchmark") {
    ankerl::nanobench::Bench().title("std::uint16_t").name("classic").run(benchmark_op<std::uint16_t, classic::byteswap_helper>);
    ankerl::nanobench::Bench().title("std::uint16_t").name("modern").run(benchmark_op<std::uint16_t, modern::byteswap_helper>);
#if PFS_BYTESWAP_HAS_INTRINSICS
    ankerl::nanobench::Bench().title("std::uint16_t").name("intrinsics").run(benchmark_op<std::uint16_t, intrinsics::byteswap_helper>);
#endif

    ankerl::nanobench::Bench().title("std::uint32_t").name("classic").run(benchmark_op<std::uint32_t, classic::byteswap_helper>);
    ankerl::nanobench::Bench().title("std::uint32_t").name("modern").run(benchmark_op<std::uint32_t, modern::byteswap_helper>);
#if PFS_BYTESWAP_HAS_INTRINSICS
    ankerl::nanobench::Bench().title("std::uint32_t").name("intrinsics").run(benchmark_op<std::uint32_t, intrinsics::byteswap_helper>);
#endif

    ankerl::nanobench::Bench().title("std::uint64_t").name("classic").run(benchmark_op<std::uint64_t, classic::byteswap_helper>);
    ankerl::nanobench::Bench().title("std::uint64_t").name("modern").run(benchmark_op<std::uint64_t, modern::byteswap_helper>);
#if PFS_BYTESWAP_HAS_INTRINSICS
    ankerl::nanobench::Bench().title("std::uint64_t").name("intrinsics").run(benchmark_op<std::uint64_t, intrinsics::byteswap_helper>);
#endif

#if defined(PFS_HAS_INT128)
    ankerl::nanobench::Bench().title("__uint128_t").name("classic").run(benchmark_op<__uint128_t, classic::byteswap_helper>);
    ankerl::nanobench::Bench().title("__uint128_t").name("modern").run(benchmark_op<__uint128_t, modern::byteswap_helper>);
#if PFS_BYTESWAP_HAS_INTRINSICS
    ankerl::nanobench::Bench().title("__uint128_t").name("intrinsics").run(benchmark_op<__uint128_t, intrinsics::byteswap_helper>);
#endif
#endif
}
