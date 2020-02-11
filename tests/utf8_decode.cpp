#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <pfs/unicode/unicode_iterator.hpp>
#include <pfs/unicode/u8_iterator.hpp>
#include <fstream>
#include <iterator>
#include <string>
#include <sstream>
#include "unicode/utf8/test_data.hpp"

template <typename OctetIt>
bool decode (const char * itertype)
{
    int ntests = sizeof (data) / sizeof (data[0]);
    bool result {true};

    for (int i = 0; i < ntests; ++i) {
        OctetIt first(iter_cast<OctetIt>(data[i].text));
        OctetIt last(iter_cast<OctetIt>(data[i].text) + data[i].len);

        size_t count = 0;
        pfs::unicode::char_t uc = 0;
        pfs::unicode::u8_input_iterator<OctetIt> inp_first(first, last);
        pfs::unicode::u8_input_iterator<OctetIt> inp_last(last);

        while (inp_first != inp_last) {
            uc = *inp_first++;
            ++count;
        }

        std::ostringstream msg;
        msg << "Decode UTF-8 using `" << itertype << "' as pointer. String `"
                    << data[i].name << "`";

        CHECK_MESSAGE(count == data[i].nchars, msg.str());

        if (count != data[i].nchars) {
            std::ostringstream desc;

            desc << "Decode UTF-8 using `" << itertype << "' as pointer. String `"
                    << data[i].name
                    << "'. Number of unicode chars "
                    << count
                    << ", expected "
                    << data[i].nchars;
            MESSAGE(desc.str());
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
        std::ifstream ifs(data[i].filename, std::ios::binary);

        std::ostringstream msg;
        msg << "Open file `" << data[i].filename << "` failure\n";
        REQUIRE_MESSAGE(ifs.is_open(), msg.str());

        std::istreambuf_iterator first(ifs);
        std::istreambuf_iterator<char> last;

        size_t count = 0;
        pfs::unicode::char_t uc = 0;
        pfs::unicode::u8_input_iterator<std::istreambuf_iterator<char>> inp_first(first, last);
        pfs::unicode::u8_input_iterator<std::istreambuf_iterator<char>> inp_last(last);

        while (inp_first != inp_last) {
            uc = *inp_first++;
            ++count;
        }

        if (count != data[i].nchars) {
            std::ostringstream msg;

            msg << "Decode UTF-8 using `istreambuf_iterator` as pointer. String `"
                    << data[i].name
                    << "'. Number of unicode chars "
                    << count
                    << ", expected "
                    << data[i].nchars;
            MESSAGE(msg.str());
            result = false;
        }
    }

    return result;
}

TEST_CASE("Decode utf8") {
    decode<unsigned char *>("unsigned char *");
    decode<const unsigned char *>("const unsigned char *");
    decode<char *>("char *");
    decode<const char *>("const char *");
    decode<std::string::iterator>("std::string::iterator");
    decode<std::string::const_iterator>("std::string::const_iterator");
    decode_files();
}
