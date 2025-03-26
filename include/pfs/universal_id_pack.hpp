////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2024-2025 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2024.04.23 Initial version.
//      2025.01.24 Removed `binary_istream_nt`.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "universal_id.hpp"
#include "endian.hpp"
#include "binary_istream.hpp"
#include "binary_ostream.hpp"
#include <cstdint>

namespace pfs {

template <endian Endianess>
void pack (binary_ostream<Endianess> & out, universal_id const & uuid)
{
    out << high(uuid) << low(uuid);
}

template <endian Endianess>
void unpack (binary_istream<Endianess> & in, universal_id & uuid)
{
    std::uint64_t h {0}, l {0};
    in >> h >> l;
    uuid = pfs::make_uuid(h, l);
}

} // namespace pfs
