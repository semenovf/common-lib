////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2024 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2024.07.04 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "filesystem.hpp"
#include "endian.hpp"
#include "binary_istream.hpp"
#include "binary_istream_nt.hpp"
#include "binary_ostream.hpp"

namespace pfs {

template <endian Endianess>
void pack (binary_ostream<Endianess> & out, filesystem::path const & p)
{
    out << filesystem::utf8_encode(p);
}

template <endian Endianess>
void unpack (binary_istream<Endianess> & in, filesystem::path & p)
{
    std::string text;
    in >> text;
    p = filesystem::utf8_decode(text);
}

template <endian Endianess>
void unpack (binary_istream_nt<Endianess> & in, filesystem::path & p)
{
    std::string text;
    in >> text;
    p = filesystem::utf8_decode(text);
}

} // namespace pfs
