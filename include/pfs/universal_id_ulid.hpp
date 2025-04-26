////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021-2024 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2021.08.14 Initial version (in `chat-lib`).
//      2021.10.01 Moved from `chat-lib`.
//      2021.10.03 Added calculation CRC32 and CRC64.
//      2021.11.08 Added `make_uuid` - UUID construction functions.
//      2024.12.23 Renamed from `universal_id.hpp`.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "assert.hpp"
#include "endian.hpp"
#include "i128.hpp"
#include "fmt.hpp"
#include "namespace.hpp"
#include "optional.hpp"
#include <array>
#include <random>

#if PFS__FORCE_ULID_STRUCT
#   include "3rdparty/ulid/ulid_struct_chrono.hh"
#else
#   include "3rdparty/ulid/ulid.hh"
#endif

PFS__NAMESPACE_BEGIN

// See https://github.com/suyash/ulid
// NOTE! Struct-based implementation assumes big-endian order.

// NOTE! `uuid_t` name conficts with macro `uuid_t` defined in MSVC's shared/rpcdece.h
class universal_id //uuid_t
{
public:
#ifdef ULIDUINT128
    ulid::ULID u {0};
#else
    ulid::ULID u {};
#endif

    universal_id () {}
    universal_id (ulid::ULID v) : u(v) {}

    std::uint64_t high () const noexcept
    {
#ifdef ULIDUINT128
        return static_cast<std::uint64_t>(u >> 64);
#else
        std::uint64_t hi = static_cast<std::uint64_t>(u.data[7])
            | static_cast<std::uint64_t>(u.data[6]) << 8
            | static_cast<std::uint64_t>(u.data[5]) << 16
            | static_cast<std::uint64_t>(u.data[4]) << 24
            | static_cast<std::uint64_t>(u.data[3]) << 32
            | static_cast<std::uint64_t>(u.data[2]) << 40
            | static_cast<std::uint64_t>(u.data[1]) << 48
            | static_cast<std::uint64_t>(u.data[0]) << 56;
        return hi;
#endif
    }

    std::uint64_t low () const noexcept
    {
#ifdef ULIDUINT128
        return static_cast<std::uint64_t>(u);
#else
        std::uint64_t lo = static_cast<std::uint64_t>(u.data[15])
            | static_cast<std::uint64_t>(u.data[14]) << 8
            | static_cast<std::uint64_t>(u.data[13]) << 16
            | static_cast<std::uint64_t>(u.data[12]) << 24
            | static_cast<std::uint64_t>(u.data[11]) << 32
            | static_cast<std::uint64_t>(u.data[10]) << 40
            | static_cast<std::uint64_t>(u.data[9])  << 48
            | static_cast<std::uint64_t>(u.data[8])  << 56;
        return lo;
#endif
    }
};

using random_engine_t = std::mt19937;

inline auto random_engine () -> random_engine_t &
{
    static std::random_device __rd; // Will be used to obtain a seed for the random number engine
    static random_engine_t result{__rd()}; // Standard mersenne_twister_engine seeded with rd()
    return result;
}

inline std::uint64_t low (universal_id const & id)
{
    return id.low();
}

inline std::uint64_t high (universal_id const & id)
{
    return id.high();
}

inline universal_id generate_uuid ()
{
    universal_id result;

    // Encode first 6 bytes with the timestamp in milliseconds using
    // std::chrono::system_clock::now()
    ulid::EncodeTimeSystemClockNow(result.u);

    ulid::EncodeEntropyMt19937(random_engine(), result.u);

    return result;
}

inline std::string to_string (universal_id const & value)
{
    return ulid::Marshal(value.u);
}

template <typename T>
T from_string (std::string const & str);

/**
 * @deprecated Use @c parse_universal_id instead
 */
template <>
[[deprecated]] inline universal_id from_string<universal_id> (std::string const & str)
{
    return str.size() == 26
        ? ulid::Unmarshal(str)
        : universal_id{};
}

inline pfs::optional<universal_id> parse_universal_id (char const * s, std::size_t n)
{
    if (n != 26)
        return pfs::nullopt;

    universal_id id;
    ulid::UnmarshalFrom(s, id.u);
    return id;
}

inline pfs::optional<universal_id> parse_universal_id (std::string const & text)
{
    return parse_universal_id(text.data(), text.size());
}

#ifdef ULIDUINT128

inline universal_id make_uuid (std::uint64_t hi, std::uint64_t lo)
{
    return pfs::universal_id{construct_uint128(hi, lo)};
}

/**
 * Makes UUID from array @a a that contains bytes in order specified by @a e.
 */
template <typename Byte>
inline universal_id make_uuid2 (std::array<Byte, 16> const & a, endian e = endian::network)
{
    universal_id result {0};

    if (e == endian::little) {
        for (int i = 0, j = 0; i < 16; i++, j += 8)
            result.u = result.u | static_cast<int128_type>(static_cast<std::uint8_t>(a[i])) << j;
    } else {
        for (int i = 15, j = 0; i >= 0; i--, j += 8)
            result.u = result.u | static_cast<int128_type>(static_cast<std::uint8_t>(a[i])) << j;
    }

    return result;
}

/**
 * Makes UUID from array @a a that contains bytes in order specified by @a e.
 */
inline universal_id make_uuid (std::array<std::uint8_t, 16> const & a, endian e = endian::network)
{
    return make_uuid2<std::uint8_t>(a, e);
}

/**
 * Converts UUID into array that will contains bytes in order specified by @a e.
 */
template <typename Byte>
inline std::array<Byte, 16> to_array2 (universal_id const & u, endian e = endian::network)
{
    std::array<Byte, 16> result;

    if (e == endian::little) {
        for (int i = 0, j = 0; i < 16; i++, j += 8)
            result[i] = static_cast<Byte>(static_cast<std::uint8_t>(u.u >> j));
    } else {
        for (int i = 0, j = 120; i < 16; i++, j -= 8)
            result[i] = static_cast<Byte>(static_cast<std::uint8_t>(u.u >> j));
    }

    return result;
}

/**
 * Converts UUID into array that will contains bytes in order specified by @a e.
 */
inline std::array<std::uint8_t, 16> to_array (universal_id const & u, endian e = endian::network)
{
    return to_array2<std::uint8_t>(u, e);
}

#else // ULIDUINT128

inline universal_id make_uuid (std::uint64_t hi, std::uint64_t lo)
{
    universal_id result;
    std::uint8_t * a = & result.u.data[0];

    a[15] = static_cast<std::uint8_t>(lo);

    lo >>= 8;
    a[14] = static_cast<uint8_t>(lo);

    lo >>= 8;
    a[13] = static_cast<uint8_t>(lo);

    lo >>= 8;
    a[12] = static_cast<uint8_t>(lo);

    lo >>= 8;
    a[11] = static_cast<uint8_t>(lo);

    lo >>= 8;
    a[10] = static_cast<uint8_t>(lo);

    lo >>= 8;
    a[9] = static_cast<uint8_t>(lo);

    lo >>= 8;
    a[8] = static_cast<uint8_t>(lo);

    a[7] = static_cast<std::uint8_t>(hi);

    hi >>= 8;
    a[6] = static_cast<uint8_t>(hi);

    hi >>= 8;
    a[5] = static_cast<uint8_t>(hi);

    hi >>= 8;
    a[4] = static_cast<uint8_t>(hi);

    hi >>= 8;
    a[3] = static_cast<uint8_t>(hi);

    hi >>= 8;
    a[2] = static_cast<uint8_t>(hi);

    hi >>= 8;
    a[1] = static_cast<uint8_t>(hi);

    hi >>= 8;
    a[0] = static_cast<uint8_t>(hi);

    return result;
}

/**
 * Makes UUID from array @a a that contains bytes in order specified by @a e.
 */
template <typename Byte>
inline universal_id make_uuid2 (std::array<Byte, 16> const & a, endian e = endian::network)
{
    universal_id result;

    // See NOTE at beginning of source
    if (e == endian::little) {
        for (std::size_t i = 0, j = a.size() - 1; i < a.size(); i++, j--)
            result.u.data[i] = static_cast<std::uint8_t>(a[j]);
    } else {
        for (std::size_t i = 0; i < a.size(); i++)
            result.u.data[i] = static_cast<std::uint8_t>(a[i]);
    }

    return result;
}

/**
 * Makes UUID from array @a a that contains bytes in order specified by @a e.
 */
inline universal_id make_uuid (std::array<std::uint8_t, 16> const & a, endian e = endian::network)
{
    return make_uuid2<std::uint8_t>(a, e);
}

/**
 * Converts UUID into array that will contains bytes in order specified by @a e.
 */
template <typename Byte>
inline std::array<Byte, 16> to_array2 (universal_id const & u, endian e = endian::network)
{
    std::array<Byte, 16> result;

    // See NOTE at beginning of source
    if (e == endian::little) {
        for (std::size_t i = 0, j = result.size() - 1; i < result.size(); i++, j--)
            result[i] = static_cast<Byte>(u.u.data[j]);
    } else {
        for (std::size_t i = 0; i < result.size(); i++)
            result[i] = static_cast<Byte>(u.u.data[i]);
    }

    return result;
}

/**
 * Converts UUID into array that will contains bytes in order specified by @a e.
 */
inline std::array<std::uint8_t, 16> to_array (universal_id const & u, endian e = endian::network)
{
    return to_array2<std::uint8_t>(u, e);
}

#endif // !ULIDUINT128

inline int compare (universal_id const & u1, universal_id const & u2)
{
    return ulid::CompareULIDs(u1.u, u2.u);
}

inline bool operator == (universal_id const & u1, universal_id const & u2)
{
    return compare(u1, u2) == 0;
}

inline bool operator != (universal_id const & u1, universal_id const & u2)
{
    return compare(u1, u2) != 0;
}

inline bool operator < (universal_id const & u1, universal_id const & u2)
{
    return compare(u1, u2) < 0;
}

inline bool operator <= (universal_id const & u1, universal_id const & u2)
{
    return compare(u1, u2) <= 0;
}

inline bool operator > (universal_id const & u1, universal_id const & u2)
{
    return compare(u1, u2) > 0;
}

inline bool operator >= (universal_id const & u1, universal_id const & u2)
{
    return compare(u1, u2) >= 0;
}

PFS__NAMESPACE_END

namespace fmt {

template <>
struct formatter<pfs::universal_id>
{
    template <typename ParseContext>
    constexpr auto parse (ParseContext & ctx) const -> decltype(ctx.begin())
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format (pfs::universal_id const & uuid, FormatContext & ctx) const -> decltype(ctx.out())
    {
        return format_to(ctx.out(), "#{}", to_string(uuid));
    }
};

} // namespace fmt

inline pfs::universal_id operator ""_uuid (char const * str, std::size_t len)
{
    PFS__ASSERT(len == 26 || len == 0, "UUID literal must be 26 characters or empty");

    if (len == 0)
        return pfs::universal_id{};

    pfs::universal_id result;
    ulid::UnmarshalFrom(str, result.u);
    return result;
}
