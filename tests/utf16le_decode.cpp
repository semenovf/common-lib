////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of [pfs-common](https://github.com/semenovf/pfs-common) library.
//
// Changelog:
//      2020.11.08 Initial version
//
////////////////////////////////////////////////////////////////////////////////
#define UTF_SUBDIR "utf16le"
#include "utf16_decode.hpp"

TEST_CASE("Decode utf16le") {
    decode<uint16_t *>("uint16 *", true);
    decode<uint16_t const *>("uint16_t const *", true);
    decode<int16_t *>("int16_t *", true);
    decode<int16_t const *>("int16_t const *", true);
    // FIXME
//     decode<std::string::iterator>("std::string::iterator");
//     decode<std::string::const_iterator>("std::string::const_iterator");
    // FIXME
//     decode_files(true);
}
