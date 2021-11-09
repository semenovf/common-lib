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
#include <random>

#if PFS_COMMON__FORCE_ULID_STRUCT
#   include "3rdparty/ulid/ulid_struct.hh"
#else
#   include "3rdparty/ulid/ulid.hh"
#endif

namespace pfs {

// See https://github.com/suyash/ulid

using uuid_t = ulid::ULID;
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
    ulid::EncodeTimeSystemClockNow(result);

    ulid::EncodeEntropyMt19937(random_engine(), result);

    return result;
}

inline std::string to_string (uuid_t const & value)
{
    return ulid::Marshal(value);
}

template <typename T>
T from_string (std::string const & str);

template <>
inline uuid_t from_string<uuid_t> (std::string const & str)
{
    return ulid::Unmarshal(str);
}

#ifdef ULID_UINT128_HH
inline pfs::uuid_t make_uuid (std::uint64_t hi, std::uint64_t lo)
{
    return pfs::uuid_t{construct_uint128(hi, lo)};
}
#else
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

// NOTE! int128 and struct -based implementations of ULID are not fully
//       interchangeable. An exception is representaion at big-endian
//       architechture, where struct-based implementations will still remain
//       little-endian.
//       So, for compatibility (e.g. when need transfer UUID over network) it is
//       the better to use struct-based implementation.
//       Or use string representation (to_string() / from_string()).

/**
 * Convert from network to native order
 */
template <typename T>
inline PFS_BYTESWAP_CONSTEXPR
T to_native_order (T const & x, typename std::enable_if<
          std::is_same<T, uuid_t>::value
        , std::nullptr_t>::type = nullptr)
{
    return x;
}

/**
 * Convert from native to network order
 */
template <typename T>
inline PFS_BYTESWAP_CONSTEXPR
T to_network_order (T const & x, typename std::enable_if<
          std::is_same<T, uuid_t>::value
        , std::nullptr_t>::type = nullptr)
{
    return x;
}
#endif // ULID_STRUCT_HH

} // namespace pfs

namespace std {

inline std::string to_string (pfs::uuid_t const & value)
{
    return pfs::to_string(value);
}

} // namespace std

#ifdef ULID_STRUCT_HH
namespace ulid {
    inline bool operator == (pfs::uuid_t const & u1, pfs::uuid_t const & u2)
    {
        return ulid::CompareULIDs(u1, u2) == 0;
    }

    inline bool operator != (pfs::uuid_t const & u1, pfs::uuid_t const & u2)
    {
        return ulid::CompareULIDs(u1, u2) != 0;
    }
} // namespace ulid
#endif

pfs::uuid_t operator ""_uuid (char const * str, std::size_t)
{
    pfs::uuid_t result;
    ulid::UnmarshalFrom(str, result);
    return result;
}
