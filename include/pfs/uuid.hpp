////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of [common-lib](https://github.com/semenovf/common-lib) library.
//
// Changelog:
//      2021.08.14 Initial version (in `chat-lib`).
//      2021.10.01 Moved from `chat-lib`.
//      2021.10.03 Added calculation CRC32 and CRC64.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "crc32.hpp"
#include "crc64.hpp"
#include "3rdparty/ulid/ulid.hh"
#include <random>

namespace pfs {

// see https://github.com/suyash/ulid

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

#ifdef ULIDUINT128
template <>
inline std::int32_t crc32_of<uuid_t> (uuid_t const & data, std::int32_t initial)
{
    return crc32_all_of(initial
        , static_cast<std::uint8_t>(data >> 120)
        , static_cast<std::uint8_t>(data >> 112)
        , static_cast<std::uint8_t>(data >> 104)
        , static_cast<std::uint8_t>(data >> 96)
        , static_cast<std::uint8_t>(data >> 88)
        , static_cast<std::uint8_t>(data >> 80)
        , static_cast<std::uint8_t>(data >> 72)
        , static_cast<std::uint8_t>(data >> 64)
        , static_cast<std::uint8_t>(data >> 56)
        , static_cast<std::uint8_t>(data >> 48)
        , static_cast<std::uint8_t>(data >> 40)
        , static_cast<std::uint8_t>(data >> 32)
        , static_cast<std::uint8_t>(data >> 24)
        , static_cast<std::uint8_t>(data >> 16)
        , static_cast<std::uint8_t>(data >> 8)
        , static_cast<std::uint8_t>(data >> 0));
}
template <>
inline std::int64_t crc64_of<uuid_t> (uuid_t const & data, std::int64_t initial)
{
    return crc64_all_of(initial
        , static_cast<std::uint8_t>(data >> 120)
        , static_cast<std::uint8_t>(data >> 112)
        , static_cast<std::uint8_t>(data >> 104)
        , static_cast<std::uint8_t>(data >> 96)
        , static_cast<std::uint8_t>(data >> 88)
        , static_cast<std::uint8_t>(data >> 80)
        , static_cast<std::uint8_t>(data >> 72)
        , static_cast<std::uint8_t>(data >> 64)
        , static_cast<std::uint8_t>(data >> 56)
        , static_cast<std::uint8_t>(data >> 48)
        , static_cast<std::uint8_t>(data >> 40)
        , static_cast<std::uint8_t>(data >> 32)
        , static_cast<std::uint8_t>(data >> 24)
        , static_cast<std::uint8_t>(data >> 16)
        , static_cast<std::uint8_t>(data >> 8)
        , static_cast<std::uint8_t>(data >> 0));
}
#else
template <>
inline std::int32_t crc32_of<uuid_t> (uuid_t const & data, std::int32_t initial)
{
    return crc32_all_of(initial
        , data.data[0]
        , data.data[1]
        , data.data[2]
        , data.data[3]
        , data.data[4]
        , data.data[5]
        , data.data[6]
        , data.data[7]
        , data.data[8]
        , data.data[9]
        , data.data[10]
        , data.data[11]
        , data.data[12]
        , data.data[13]
        , data.data[14]
        , data.data[15]);
}

template <>
inline std::int64_t crc64_of<uuid_t> (uuid_t const & data, std::int64_t initial)
{
    return crc64_all_of(initial
        , data.data[0]
        , data.data[1]
        , data.data[2]
        , data.data[3]
        , data.data[4]
        , data.data[5]
        , data.data[6]
        , data.data[7]
        , data.data[8]
        , data.data[9]
        , data.data[10]
        , data.data[11]
        , data.data[12]
        , data.data[13]
        , data.data[14]
        , data.data[15]);
}

#endif

} // namespace pfs

namespace std {

inline std::string to_string (pfs::uuid_t const & value)
{
    return pfs::to_string(value);
}

} // namespace std
