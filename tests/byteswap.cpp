////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of [common-lib](https://github.com/semenovf/common-lib) library.
//
// Changelog:
//      2021.10.03 Initial version.
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define ANKERL_NANOBENCH_IMPLEMENT
#include "doctest.h"
#include "nanobench.h"
#include "pfs/bits/compiler.h"
#include <algorithm>
#include <cassert>
#include <cstdint>

#if PFS_COMPILER_MSC
#   include "intrin.h"
#endif

#if __cplusplus >= 201402L
#   define PFS_BYTESWAP_CONSTEXPR constexpr
#else
#   define PFS_BYTESWAP_CONSTEXPR
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

#if PFS_COMPILER_GNUC
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

// template <>
// inline PFS_BYTESWAP_CONSTEXPR unsigned __int128 byteswap<unsigned __int128> (unsigned __int128 x) noexcept
// {
//     return __builtin_bswap128(x);
// }

#elif PFS_COMPILER_MSC
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
        || std::is_same<T, __int128>::value
        || std::is_same<T, unsigned __int128>::value
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

TEST_CASE("benchmark") {
    ankerl::nanobench::Bench().title("std::uint16_t").name("classic").run(benchmark_op<std::uint16_t, classic::byteswap_helper>);
    ankerl::nanobench::Bench().title("std::uint16_t").name("modern").run(benchmark_op<std::uint16_t, modern::byteswap_helper>);
#if PFS_COMPILER_GNUC //|| PFS_COMPILER_MSC
    ankerl::nanobench::Bench().title("std::uint16_t").name("intrinsics").run(benchmark_op<std::uint16_t, intrinsics::byteswap_helper>);
#endif

    ankerl::nanobench::Bench().title("std::uint32_t").name("classic").run(benchmark_op<std::uint32_t, classic::byteswap_helper>);
    ankerl::nanobench::Bench().title("std::uint32_t").name("modern").run(benchmark_op<std::uint32_t, modern::byteswap_helper>);
#if PFS_COMPILER_GNUC //|| PFS_COMPILER_MSC
    ankerl::nanobench::Bench().title("std::uint32_t").name("intrinsics").run(benchmark_op<std::uint32_t, intrinsics::byteswap_helper>);
#endif

    ankerl::nanobench::Bench().title("std::uint64_t").name("classic").run(benchmark_op<std::uint64_t, classic::byteswap_helper>);
    ankerl::nanobench::Bench().title("std::uint64_t").name("modern").run(benchmark_op<std::uint64_t, modern::byteswap_helper>);
#if PFS_COMPILER_GNUC //|| PFS_COMPILER_MSC
    ankerl::nanobench::Bench().title("std::uint64_t").name("intrinsics").run(benchmark_op<std::uint64_t, intrinsics::byteswap_helper>);
#endif

#if defined(__SIZEOF_INT128__)
    ankerl::nanobench::Bench().title("__int128").name("modern").run(benchmark_op<__int128, modern::byteswap_helper>);
    ankerl::nanobench::Bench().title("unsigned __int128").name("modern").run(benchmark_op<unsigned __int128, modern::byteswap_helper>);
#endif
}
