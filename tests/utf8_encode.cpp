////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2020 Vladislav Trifochkin
//
// This file is part of [common-lib](https://github.com/semenovf/common-lib) library.
//
// Changelog:
//
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <pfs/unicode/unicode_iterator.hpp>
#include <pfs/unicode/u8_iterator.hpp>
#include <string>
#include <sstream>

#define UTF_SUBDIR "utf8"
#include "unicode/test_data.hpp"

template <typename OctetIt>
bool encode (char const * itertype)
{
    int ntests = sizeof (data) / sizeof (data[0]);
    bool result {true};

    for (int i = 0; i < ntests; ++i) {
        OctetIt first(iter_cast<OctetIt>(data[i].text));
        OctetIt last(iter_cast<OctetIt>(data[i].text) + data[i].len);
        pfs::unicode::u8_input_iterator<OctetIt> inp_first(first, last);
        pfs::unicode::u8_input_iterator<OctetIt> inp_last(last);

        pfs::unicode::char_t uc(0);
        std::string str;

        while (inp_first != inp_last) {
            uc = *inp_first++;
            pfs::unicode::u8_output_iterator<std::back_insert_iterator<std::string>> out(std::back_inserter(str));
            *out++ = uc;
        }

        auto sample = std::string(reinterpret_cast<char *>(data[i].text), data[i].len);

        if (str != sample) {
            std::ostringstream desc;

            desc << "Decode/Encode to UTF-8 using `"
                    << itertype << "' as pointer. String `"
                    << data[i].name << "'.";

            MESSAGE(desc.str());

            result = false;
        }
    }

    return result;
}

TEST_CASE("Encode utf8") {
    CHECK(encode<unsigned char *>("unsigned char *"));
    CHECK(encode<const unsigned char *>("const unsigned char *"));
    CHECK(encode<char *>("char *"));
    CHECK(encode<const char *>("const char *"));

    // FIXME for Windows
    // CHECK(encode<std::string::iterator>("std::string::iterator"));
    // CHECK(encode<std::string::const_iterator>("std::string::const_iterator"));
}
