#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/fmt.hpp"
#include "pfs/unicode/utf8_iterator.hpp"
#include <fstream>
#include <iterator>
#include <string>

#define UTF_SUBDIR "utf8"
#include "unicode/test_data.hpp"

template <typename OctetIt>
bool decode (const char * itertype)
{
    int ntests = sizeof (data) / sizeof (data[0]);
    bool result {true};

    for (int i = 0; i < ntests; ++i) {
        OctetIt first(iter_cast<OctetIt>(data[i].text));
        OctetIt last(first + data[i].len);

        size_t count = 0;
        pfs::unicode::char_t uc = 0;
        auto inp_first = pfs::unicode::utf8_iterator<OctetIt>::begin(first, last);
        auto inp_last  = pfs::unicode::utf8_iterator<OctetIt>::end(last);

        while (inp_first != inp_last) {
            uc = *inp_first++;
            ++count;
        }

        CHECK_MESSAGE(count == data[i].nchars
            , fmt::format("Decode UTF-8 using `{}' as pointer. String `{}`"
                , itertype, data[i].name));

        if (count != data[i].nchars) {
            auto desc = fmt::format("Decode UTF-8 using `{}' as pointer."
                " String `{}'. Number of unicode chars {}, expected {}"
                , itertype, data[i].name, count, data[i].nchars);
            MESSAGE(desc);
            result = false;
        }
    }

    return result;
}

bool decode_files ()
{
    int ntests = sizeof (data) / sizeof (data[0]);
    bool result {true};

    for (int i = 0; i < ntests; ++i) {
        {
            std::ifstream ifs(data[i].filename, std::ios::binary);

            REQUIRE_MESSAGE(ifs.is_open()
                , fmt::format("Open file `{}` failure\n", data[i].filename));

            std::istreambuf_iterator<char> first(ifs);
            std::istreambuf_iterator<char> last;

            auto inp_first = pfs::unicode::utf8_iterator<std::istreambuf_iterator<char>>::begin(first, last);
            auto inp_last  = pfs::unicode::utf8_iterator<std::istreambuf_iterator<char>>::end(last);

            CHECK_EQ(std::distance(inp_first, inp_last), data[i].nchars);
        }

        {
            std::ifstream ifs(data[i].filename, std::ios::binary);

            REQUIRE_MESSAGE(ifs.is_open()
                , fmt::format("Open file `{}` failure\n", data[i].filename));

            std::istreambuf_iterator<char> first(ifs);
            std::istreambuf_iterator<char> last;

            size_t count = 0;
            auto inp_first = pfs::unicode::utf8_iterator<std::istreambuf_iterator<char>>::begin(first, last);
            auto inp_last  = pfs::unicode::utf8_iterator<std::istreambuf_iterator<char>>::end(last);

            while (inp_first != inp_last) {
                ++inp_first;
                ++count;
            }

            if (count != data[i].nchars) {
                auto msg = fmt::format("Decode UTF-8 using `istreambuf_iterator`"
                    " as pointer. String `{}'. Number of unicode chars {}, expected {}"
                    , data[i].name, count, data[i].nchars);
                MESSAGE(msg);
                result = false;
            }
        }
    }

    return result;
}

TEST_CASE("Decode utf8") {
    decode<const char *>("const char *");
    decode<char *>("char *");
    decode<unsigned char *>("unsigned char *");
    decode<const unsigned char *>("const unsigned char *");
    decode<std::string::iterator>("std::string::iterator");
    decode<std::string::const_iterator>("std::string::const_iterator");

    decode_files();
}
