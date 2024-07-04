////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2024 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2024.04.25 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "time_point.hpp"
#include "endian.hpp"
#include "binary_istream.hpp"
#include "binary_istream_nt.hpp"
#include "binary_ostream.hpp"

namespace pfs {

template <endian Endianess>
void pack (binary_ostream<Endianess> & out, utc_time const & t)
{
    // Milliseconds is equivalent to integer type of at least 45 bits,
    // so nearest standard integer type is std::int64_t
    std::int64_t ticks = t.to_millis().count();
    out << ticks;
}

template <endian Endianess>
void unpack (binary_istream<Endianess> & in, utc_time & t)
{
    std::int64_t ticks;
    in >> ticks;
    t = utc_time{std::chrono::milliseconds{static_cast<std::chrono::milliseconds::rep>(ticks)}};
}

template <endian Endianess>
void unpack (binary_istream_nt<Endianess> & in, utc_time & t)
{
    std::int64_t ticks;
    in >> ticks;
    t = utc_time{std::chrono::milliseconds{static_cast<std::chrono::milliseconds::rep>(ticks)}};
}

template <endian Endianess>
void pack (binary_ostream<Endianess> & out, local_time const & t)
{
    // Milliseconds is equivalent to integer type of at least 45 bits,
    // so nearest standard integer type is std::int64_t
    std::int64_t ticks = t.to_millis().count();
    out << ticks;
}

template <endian Endianess>
void unpack (binary_istream<Endianess> & in, local_time & t)
{
    std::int64_t ticks;
    in >> ticks;
    t = local_time{std::chrono::milliseconds{static_cast<std::chrono::milliseconds::rep>(ticks)}};
}

template <endian Endianess>
void unpack (binary_istream_nt<Endianess> & in, local_time & t)
{
    std::int64_t ticks;
    in >> ticks;
    t = local_time{std::chrono::milliseconds{static_cast<std::chrono::milliseconds::rep>(ticks)}};
}

} // namespace pfs
