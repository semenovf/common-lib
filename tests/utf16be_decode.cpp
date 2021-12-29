////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2020.11.08 Initial version
//
////////////////////////////////////////////////////////////////////////////////
#define UTF_SUBDIR "utf16be"
#include "utf16_decode.hpp"

TEST_CASE("Decode utf16be") {
    decode<uint16_t *>("uint16 *", false);
    decode<uint16_t const *>("uint16_t const *", false);
    decode<int16_t *>("int16_t *", false);
    decode<int16_t const *>("int16_t const *", false);
    // FIXME
//     decode<std::basic_string<int16_t>::iterator>("std::basic_string<int16_t>::iterator", false);
//     decode<std::basic_string<int16_t>::const_iterator>("std::basic_string<int16_t>::const_iterator");
    // FIXME
//     decode_files(false);
}
