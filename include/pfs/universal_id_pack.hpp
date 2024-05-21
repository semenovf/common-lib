////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2024 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2024.04.23 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "universal_id.hpp"
#include "endian.hpp"
#include "binary_istream.hpp"
#include "binary_istream_nt.hpp"
#include "binary_ostream.hpp"

namespace pfs {

template <endian Endianess>
void pack (binary_ostream<Endianess> & out, universal_id const & uuid)
{
    auto a = pfs::to_array2<char>(uuid, Endianess);
    out << exclude_size{} << a;
}

template <endian Endianess>
void unpack (binary_istream<Endianess> & in, universal_id & uuid)
{
    std::array<char, 16> a;
    in >> expected_size{16} >> a;
    uuid = pfs::make_uuid2<char>(a, Endianess);
}

template <endian Endianess>
void unpack (binary_istream_nt<Endianess> & in, universal_id & uuid)
{
    std::array<char, 16> a;
    in >> expected_size{16} >> a;
    uuid = pfs::make_uuid2<char>(a, Endianess);
}

} // namespace pfs
