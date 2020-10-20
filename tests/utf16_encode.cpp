#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <pfs/unicode/unicode_iterator.hpp>
#include <pfs/unicode/u16_iterator.hpp>
#include <string>
#include <sstream>
#include "unicode/utf16/test_data.hpp"

template <typename HextetIt>
bool encode (char const* itertype)
{
    int ntests = sizeof(data) / sizeof(data[0]);
    bool result{ true };

    for (int i = 0; i < ntests; ++i) {
        HextetIt first(iter_cast<HextetIt>(data[i].text));
        HextetIt last(iter_cast<HextetIt>(data[i].text) + data[i].len);
        pfs::unicode::u16_input_iterator<HextetIt> inp_first(first, last);
        pfs::unicode::u16_input_iterator<HextetIt> inp_last(last);

        pfs::unicode::char_t uc(0);
        std::string str;

        while (inp_first != inp_last) {
            uc = *inp_first++;
            pfs::unicode::u16_output_iterator<std::back_insert_iterator<std::wstring>> out(std::back_inserter(str));
            *out++ = uc;
        }

        auto sample = std::string(reinterpret_cast<char*>(data[i].text), data[i].len);

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

TEST_CASE("Encode utf16") {
    CHECK(encode<unsigned char16_t *>("unsigned char16_t *"));
    CHECK(encode<const unsigned char*>("const unsigned char *"));
    CHECK(encode<char*>("char *"));
    CHECK(encode<const char*>("const char *"));
    CHECK(encode<std::string::iterator>("std::string::iterator"));
    CHECK(encode<std::string::const_iterator>("std::string::const_iterator"));
}
