////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2024-2025 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2024.07.04 Initial version.
//      2025.08.11 pack/upack() functions are deprecated.
//      2025.08.12 Removed deprecated functions.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "c++support.hpp"
#include "filesystem.hpp"
#include "endian.hpp"
#include "binary_istream.hpp"
#include "binary_ostream.hpp"

namespace pfs {

template <endian Endianess, typename Archive>
inline void pack (binary_ostream<Endianess, Archive> & out, filesystem::path const & p)
{
    auto text = utf8_encode_path(p);
    out << pfs::numeric_cast<std::uint16_t>(text.size()) << text;
}

template <endian Endianess>
void unpack (binary_istream<Endianess> & in, filesystem::path & p)
{
    std::string text;
    std::uint16_t sz = 0;

    in.start_transaction();
    in >> sz >> std::make_pair(& text, sz);

    if (in.commit_transaction())
        p = utf8_decode_path(text);
    else
        in.rollback_transaction();
}

} // namespace pfs
