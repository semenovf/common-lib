////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of [common-lib](https://github.com/semenovf/common-lib) library.
//
// Changelog:
//      2021.10.17 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "crc16.hpp"
#include "crc32.hpp"
#include "crc64.hpp"
#include "uuid.hpp"

namespace pfs {

#ifdef ULIDUINT128
template <>
inline std::int16_t crc16_of<uuid_t> (uuid_t const & data, std::int16_t initial)
{
    return crc16_all_of(initial
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
#else // ULIDUINT128
template <>
inline std::int16_t crc16_of<uuid_t> (uuid_t const & data, std::int16_t initial)
{
    return crc16_all_of(initial
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

#endif // ! ULIDUINT128

} // namespace pfs
