////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of [common-lib](https://github.com/semenovf/common-lib) library.
//
// Changelog:
//      2021.08.14 Initial version (in `chat-lib`).
//      2021.10.01 Moved from `chat-lib`.
//      2021.10.03 Added calculation CRC32 and CRC64.
//      2021.11.08 Added `make_uuid` - UUID construction functions.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "endian.hpp"
#include "i128.hpp"
#include <array>
#include <random>

#if PFS_COMMON__FORCE_ULID_STRUCT
#   include "3rdparty/ulid/ulid_struct.hh"
#else
#   include "3rdparty/ulid/ulid.hh"
#endif

namespace pfs {

// See https://github.com/suyash/ulid
// NOTE! Struct-based implementation assumes big-endian order.

#ifdef ULIDUINT128
    struct uuid_t
    {
        ulid::ULID u;
    };
#else // ULIDUINT128
    using uuid_t = ulid::ULID;
#endif // ! ULIDUINT128

using random_engine_t = std::mt19937;

inline auto random_engine () -> random_engine_t &
{
    static std::random_device __rd; // Will be used to obtain a seed for the random number engine
    static random_engine_t result{__rd()}; // Standard mersenne_twister_engine seeded with rd()
    return result;
}

inline uuid_t generate_uuid ()
{
    uuid_t result;

    // Encode first 6 bytes with the timestamp in milliseconds using
    // std::chrono::system_clock::now()
#ifdef ULIDUINT128
    ulid::EncodeTimeSystemClockNow(result.u);
#else
    ulid::EncodeTimeSystemClockNow(result);
#endif

#ifdef ULIDUINT128
    ulid::EncodeEntropyMt19937(random_engine(), result.u);
#else
    ulid::EncodeEntropyMt19937(random_engine(), result);
#endif

    return result;
}

inline std::string to_string (uuid_t const & value)
{
#ifdef ULIDUINT128
    return ulid::Marshal(value.u);
#else
    return ulid::Marshal(value);
#endif
}

template <typename T>
T from_string (std::string const & str);

template <>
inline uuid_t from_string<uuid_t> (std::string const & str)
{
#ifdef ULIDUINT128
    return uuid_t {ulid::Unmarshal(str)};
#else
    return ulid::Unmarshal(str);
#endif
}

#ifdef ULIDUINT128

inline uuid_t make_uuid (std::uint64_t hi, std::uint64_t lo)
{
    return pfs::uuid_t{construct_uint128(hi, lo)};
}

/**
 * Makes UUID from array @a a that contains bytes in order specified by @a e.
 */
inline pfs::uuid_t make_uuid (std::array<std::uint8_t, 16> const & a
    , endian e = endian::network)
{
    pfs::uuid_t result {0};

    if (e == endian::little) {
        for (int i = 0, j = 0; i < 16; i++, j += 8)
            result.u = result.u | static_cast<int128_type>(a[i]) << j;
    } else {
        for (int i = 15, j = 0; i >= 0; i--, j += 8)
            result.u = result.u | static_cast<int128_type>(a[i]) << j;
    }

    return result;
}

/**
 * Converts UUID into array that will contains bytes in order specified by @a e.
 */
inline std::array<std::uint8_t, 16> to_array (uuid_t const & u
    , endian e = endian::network)
{
    std::array<std::uint8_t, 16> result;

    if (e == endian::little) {
        for (int i = 0, j = 0; i < 16; i++, j += 8)
            result[i] = static_cast<std::uint8_t>(u.u >> j);
    } else {
        for (int i = 0, j = 120; i < 16; i++, j -= 8)
            result[i] = static_cast<std::uint8_t>(u.u >> j);
    }

    return result;
}

#else // ULIDUINT128

inline pfs::uuid_t make_uuid (std::uint64_t hi, std::uint64_t lo)
{
    pfs::uuid_t result;
    std::uint8_t * a = & result.data[0];

    a[15] = static_cast<std::uint8_t>(hi);

    hi >>= 8;
    a[14] = static_cast<uint8_t>(hi);

    hi >>= 8;
    a[13] = static_cast<uint8_t>(hi);

    hi >>= 8;
    a[12] = static_cast<uint8_t>(hi);

    hi >>= 8;
    a[11] = static_cast<uint8_t>(hi);

    hi >>= 8;
    a[10] = static_cast<uint8_t>(hi);

    hi >>= 8;
    a[9] = static_cast<uint8_t>(hi);

    hi >>= 8;
    a[8] = static_cast<uint8_t>(hi);

    a[7] = static_cast<std::uint8_t>(lo);

    lo >>= 8;
    a[6] = static_cast<uint8_t>(lo);

    lo >>= 8;
    a[5] = static_cast<uint8_t>(lo);

    lo >>= 8;
    a[4] = static_cast<uint8_t>(lo);

    lo >>= 8;
    a[3] = static_cast<uint8_t>(lo);

    lo >>= 8;
    a[2] = static_cast<uint8_t>(lo);

    lo >>= 8;
    a[1] = static_cast<uint8_t>(lo);

    lo >>= 8;
    a[0] = static_cast<uint8_t>(lo);

    return result;
}

/**
 * Makes UUID from array @a a that contains bytes in order specified by @a e.
 */
inline pfs::uuid_t make_uuid (std::array<std::uint8_t, 16> const & a
    , endian e = endian::network)
{
    pfs::uuid_t result;

    // See NOTE at beginning of source
    if (e == endian::little) {
        for (int i = 0, j = a.size() - 1; i < a.size(); i++, j--)
            result.data[i] = a[j];
    } else {
        for (int i = 0; i < a.size(); i++)
            result.data[i] = a[i];
    }

    return result;
}

/**
 * Converts UUID into array that will contains bytes in order specified by @a e.
 */
inline std::array<std::uint8_t, 16> to_array (uuid_t const & u
    , endian e = endian::network)
{
    std::array<std::uint8_t, 16> result;

    // See NOTE at beginning of source
    if (e == endian::little) {
        for (int i = 0, j = result.size() - 1; i < result.size(); i++, j--)
            result[i] = u.data[j];
    } else {
        for (int i = 0; i < result.size(); i++)
            result[i] = u.data[i];
    }

    return result;
}

#endif // !ULIDUINT128

inline int compare (pfs::uuid_t const & u1, pfs::uuid_t const & u2)
{
#ifdef ULIDUINT128
    return ulid::CompareULIDs(u1.u, u2.u);
#else
    return ulid::CompareULIDs(u1, u2);
#endif
}

} // namespace pfs

namespace std {

inline std::string to_string (pfs::uuid_t const & value)
{
    return pfs::to_string(value);
}

} // namespace std

inline bool operator == (pfs::uuid_t const & u1, pfs::uuid_t const & u2)
{
    return pfs::compare(u1, u2) == 0;
}

inline bool operator != (pfs::uuid_t const & u1, pfs::uuid_t const & u2)
{
    return pfs::compare(u1, u2) != 0;
}

inline bool operator < (pfs::uuid_t const & u1, pfs::uuid_t const & u2)
{
    return pfs::compare(u1, u2) < 0;
}

inline bool operator <= (pfs::uuid_t const & u1, pfs::uuid_t const & u2)
{
    return pfs::compare(u1, u2) <= 0;
}

inline bool operator > (pfs::uuid_t const & u1, pfs::uuid_t const & u2)
{
    return pfs::compare(u1, u2) > 0;
}

inline bool operator >= (pfs::uuid_t const & u1, pfs::uuid_t const & u2)
{
    return pfs::compare(u1, u2) >= 0;
}

pfs::uuid_t operator ""_uuid (char const * str, std::size_t)
{
    pfs::uuid_t result;
#ifdef ULIDUINT128
    ulid::UnmarshalFrom(str, result.u);
#else
    ulid::UnmarshalFrom(str, result);
#endif
    return result;
}
