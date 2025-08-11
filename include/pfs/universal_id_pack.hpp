////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2024-2025 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2024.04.23 Initial version.
//      2025.01.24 Removed `binary_istream_nt`.
//      2025.08.11 pack/upack() functions are deprecated.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "c++support.hpp"
#include "universal_id.hpp"
#include "endian.hpp"
#include "binary_istream.hpp"
#include "binary_ostream.hpp"
#include <cstdint>

namespace pfs {

template <endian Endianess>
PFS__DEPRECATED inline void pack (v1::binary_ostream<Endianess> & out, universal_id const & uuid)
{
    out << high(uuid) << low(uuid);
}

template <endian Endianess>
PFS__DEPRECATED inline void unpack (v1::binary_istream<Endianess> & in, universal_id & uuid)
{
    std::uint64_t h {0}, l {0};
    in >> h >> l;
    uuid = make_uuid(h, l);
}

namespace v2 {

template <endian Endianess, typename Archive>
inline void pack (binary_ostream<Endianess, Archive> & out, universal_id const & uuid)
{
    if (Endianess == endian::big)
        out << high(uuid) << low(uuid);
    else
        out << low(uuid) << high(uuid);
}

template <endian Endianess>
inline void unpack (binary_istream<Endianess> & in, universal_id & uuid)
{
    std::uint64_t h {0}, l {0};

    if (Endianess == endian::big)
        in >> h >> l;
    else
        in >> l >> h;

    uuid = make_uuid(h, l);
}

} // namespace v2

} // namespace pfs
