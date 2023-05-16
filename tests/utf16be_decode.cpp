////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020-2023 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2020.11.08 Initial version
//      2022.05.16 Fixed tests for std::basic_string<>::iterator family.
////////////////////////////////////////////////////////////////////////////////
#define UTF_SUBDIR "utf16be"
#include "utf16_decode.hpp"

TEST_CASE("Decode utf16be") {
    decode<std::uint16_t *, pfs::unicode::utf16be_byteswap>("uint16 *");
    decode<std::uint16_t const *, pfs::unicode::utf16be_byteswap>("uint16_t const *");
    decode<std::int16_t *, pfs::unicode::utf16be_byteswap>("int16_t *");
    decode<std::int16_t const *, pfs::unicode::utf16be_byteswap>("int16_t const *");

    decode<std::basic_string<std::int16_t>::iterator, pfs::unicode::utf16be_byteswap>("std::basic_string<int16_t>::iterator");
    decode<std::basic_string<std::int16_t>::const_iterator, pfs::unicode::utf16be_byteswap>("std::basic_string<int16_t>::const_iterator");
    decode<std::basic_string<std::uint16_t>::iterator, pfs::unicode::utf16be_byteswap>("std::basic_string<uint16_t>::iterator");
    decode<std::basic_string<std::uint16_t>::const_iterator, pfs::unicode::utf16be_byteswap>("std::basic_string<uint16_t>::const_iterator");

    // FIXME see decode_files() definition
    // decode_files<pfs::unicode::utf16le_byteswap>();
}
