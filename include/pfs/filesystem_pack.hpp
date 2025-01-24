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
#include "binary_ostream.hpp"

namespace pfs {

template <endian Endianess>
void pack (binary_ostream<Endianess> & out, filesystem::path const & p)
{
    auto text = filesystem::utf8_encode(p);
    out << std::make_pair(text.data(), pfs::numeric_cast<std::uint16_t>(text.size()));
}

template <endian Endianess>
void unpack (binary_istream<Endianess> & in, filesystem::path & p)
{
    std::string text;
    std::uint16_t sz;
    in.start_transaction();
    in >> sz >> std::make_pair(& text, & sz);

    if (in.commit_transaction())
        p = filesystem::utf8_decode(text);
}

} // namespace pfs
