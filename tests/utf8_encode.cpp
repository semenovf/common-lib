    ////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2023 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/fmt.hpp"
#include "pfs/unicode/utf8_iterator.hpp"
#include <string>

#define UTF_SUBDIR "utf8"
#include "unicode/test_data.hpp"

template <typename OctetIt>
bool encode (char const * itertype)
{
    int ntests = sizeof (data) / sizeof (data[0]);
    bool result {true};

    for (int i = 0; i < ntests; ++i) {
        OctetIt first(iter_cast<OctetIt>(data[i].text));
        OctetIt last(first + data[i].len);
        auto inp_first = pfs::unicode::utf8_iterator<OctetIt>::begin(first, last);
        auto inp_last  = pfs::unicode::utf8_iterator<OctetIt>::end(last);

        pfs::unicode::char_t uc(0);
        std::string str;

        while (inp_first != inp_last) {
            uc = *inp_first++;
            pfs::unicode::utf8_output_iterator<std::back_insert_iterator<std::string>> out(std::back_inserter(str));
            *out++ = uc;
        }

        auto sample = std::string(reinterpret_cast<char *>(data[i].text), data[i].len);

        if (str != sample) {
            MESSAGE(fmt::format("Decode/Encode to UTF-8 using `{}' as pointer. String `{}'."
                , itertype, data[i].name));
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
    CHECK(encode<std::string::iterator>("std::string::iterator"));
    CHECK(encode<std::string::const_iterator>("std::string::const_iterator"));
}
