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

} // namespace pfs

namespace std {

inline std::string to_string (pfs::uuid_t const & value)
{
    return pfs::to_string(value);
}

} // namespace std
