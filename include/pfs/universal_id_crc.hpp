////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2021.10.17 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "crc16.hpp"
#include "crc32.hpp"
#include "crc64.hpp"
#include "universal_id.hpp"

namespace pfs {

#if defined(PFS__UNIVERSAL_ID_IMPL_UUIDV7)
template <>
inline std::int16_t crc16_of<universal_id> (universal_id const & data, std::int16_t initial)
{
    auto a = data.to_array();
    return crc16_all_of(initial
        , a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8]
        , a[9], a[10], a[11], a[12], a[13], a[14], a[15]);
}

template <>
inline std::int32_t crc32_of<universal_id> (universal_id const & data, std::int32_t initial)
{
    auto a = data.to_array();
    return crc32_all_of(initial
        , a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8]
        , a[9], a[10], a[11], a[12], a[13], a[14], a[15]);
}

template <>
inline std::int64_t crc64_of<universal_id> (universal_id const & data, std::int64_t initial)
{
    auto a = data.to_array();
    return crc64_all_of(initial
        , a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9]
        , a[10], a[11], a[12], a[13], a[14], a[15]);
}
#else // !PFS__UNIVERSAL_ID_IMPL_UUIDV7
#   ifdef ULIDUINT128
template <>
inline std::int16_t crc16_of<universal_id> (universal_id const & data, std::int16_t initial)
{
    return crc16_all_of(initial
        , static_cast<std::uint8_t>(data.u >> 120)
        , static_cast<std::uint8_t>(data.u >> 112)
        , static_cast<std::uint8_t>(data.u >> 104)
        , static_cast<std::uint8_t>(data.u >> 96)
        , static_cast<std::uint8_t>(data.u >> 88)
        , static_cast<std::uint8_t>(data.u >> 80)
        , static_cast<std::uint8_t>(data.u >> 72)
        , static_cast<std::uint8_t>(data.u >> 64)
        , static_cast<std::uint8_t>(data.u >> 56)
        , static_cast<std::uint8_t>(data.u >> 48)
        , static_cast<std::uint8_t>(data.u >> 40)
        , static_cast<std::uint8_t>(data.u >> 32)
        , static_cast<std::uint8_t>(data.u >> 24)
        , static_cast<std::uint8_t>(data.u >> 16)
        , static_cast<std::uint8_t>(data.u >> 8)
        , static_cast<std::uint8_t>(data.u >> 0));
}

template <>
inline std::int32_t crc32_of<universal_id> (universal_id const & data, std::int32_t initial)
{
    return crc32_all_of(initial
        , static_cast<std::uint8_t>(data.u >> 120)
        , static_cast<std::uint8_t>(data.u >> 112)
        , static_cast<std::uint8_t>(data.u >> 104)
        , static_cast<std::uint8_t>(data.u >> 96)
        , static_cast<std::uint8_t>(data.u >> 88)
        , static_cast<std::uint8_t>(data.u >> 80)
        , static_cast<std::uint8_t>(data.u >> 72)
        , static_cast<std::uint8_t>(data.u >> 64)
        , static_cast<std::uint8_t>(data.u >> 56)
        , static_cast<std::uint8_t>(data.u >> 48)
        , static_cast<std::uint8_t>(data.u >> 40)
        , static_cast<std::uint8_t>(data.u >> 32)
        , static_cast<std::uint8_t>(data.u >> 24)
        , static_cast<std::uint8_t>(data.u >> 16)
        , static_cast<std::uint8_t>(data.u >> 8)
        , static_cast<std::uint8_t>(data.u >> 0));
}

template <>
inline std::int64_t crc64_of<universal_id> (universal_id const & data, std::int64_t initial)
{
    return crc64_all_of(initial
        , static_cast<std::uint8_t>(data.u >> 120)
        , static_cast<std::uint8_t>(data.u >> 112)
        , static_cast<std::uint8_t>(data.u >> 104)
        , static_cast<std::uint8_t>(data.u >> 96)
        , static_cast<std::uint8_t>(data.u >> 88)
        , static_cast<std::uint8_t>(data.u >> 80)
        , static_cast<std::uint8_t>(data.u >> 72)
        , static_cast<std::uint8_t>(data.u >> 64)
        , static_cast<std::uint8_t>(data.u >> 56)
        , static_cast<std::uint8_t>(data.u >> 48)
        , static_cast<std::uint8_t>(data.u >> 40)
        , static_cast<std::uint8_t>(data.u >> 32)
        , static_cast<std::uint8_t>(data.u >> 24)
        , static_cast<std::uint8_t>(data.u >> 16)
        , static_cast<std::uint8_t>(data.u >> 8)
        , static_cast<std::uint8_t>(data.u >> 0));
}
#   else // ULIDUINT128
template <>
inline std::int16_t crc16_of<universal_id> (universal_id const & data, std::int16_t initial)
{
    return crc16_all_of(initial
        , data.u.data[0]
        , data.u.data[1]
        , data.u.data[2]
        , data.u.data[3]
        , data.u.data[4]
        , data.u.data[5]
        , data.u.data[6]
        , data.u.data[7]
        , data.u.data[8]
        , data.u.data[9]
        , data.u.data[10]
        , data.u.data[11]
        , data.u.data[12]
        , data.u.data[13]
        , data.u.data[14]
        , data.u.data[15]);
}

template <>
inline std::int32_t crc32_of<universal_id> (universal_id const & data, std::int32_t initial)
{
    return crc32_all_of(initial
        , data.u.data[0]
        , data.u.data[1]
        , data.u.data[2]
        , data.u.data[3]
        , data.u.data[4]
        , data.u.data[5]
        , data.u.data[6]
        , data.u.data[7]
        , data.u.data[8]
        , data.u.data[9]
        , data.u.data[10]
        , data.u.data[11]
        , data.u.data[12]
        , data.u.data[13]
        , data.u.data[14]
        , data.u.data[15]);
}

template <>
inline std::int64_t crc64_of<universal_id> (universal_id const & data, std::int64_t initial)
{
    return crc64_all_of(initial
        , data.u.data[0]
        , data.u.data[1]
        , data.u.data[2]
        , data.u.data[3]
        , data.u.data[4]
        , data.u.data[5]
        , data.u.data[6]
        , data.u.data[7]
        , data.u.data[8]
        , data.u.data[9]
        , data.u.data[10]
        , data.u.data[11]
        , data.u.data[12]
        , data.u.data[13]
        , data.u.data[14]
        , data.u.data[15]);
}
#   endif // ! ULIDUINT128
#endif // PFS__UNIVERSAL_ID_IMPL_UUIDV7
} // namespace pfs
