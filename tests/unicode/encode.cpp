#include <pfs/test.hpp>
#include <pfs/unicode/unicode_iterator.hpp>
#include <pfs/unicode/u8_iterator.hpp>
#include <string>
#include <sstream>
#include "test_data.hpp"

template <typename OctetIt>
void __test_encode_obsolete (char const * itertype)
{
    int ntests = sizeof (data) / sizeof (data[0]);
    ADD_TESTS(ntests);

    for (int i = 0; i < ntests; ++i) {
        OctetIt it(iter_cast<OctetIt>(data[i].text));
        OctetIt it_end(iter_cast<OctetIt>(data[i].text) + data[i].len);

        pfs::unicode::char_t uc(0);
        std::string result;

        while (it < it_end) {
            uc = pfs::unicode::utf8_iterator<OctetIt>::decode(it);
            pfs::unicode::utf8_iterator<OctetIt>::encode(uc, pfs::back_inserter(result));
        }

        std::ostringstream desc;

        desc << "Decode/Encode to UTF-8 using `" << itertype << "' as pointer. String `"
                << data[i].name << "'.";
        TEST_OK2(result == std::string(reinterpret_cast<char *> (data[i].text), data[i].len)
                , desc.str().c_str());
    }
}

template <typename OctetIt>
void __test_encode (char const * itertype)
{
    int ntests = sizeof (data) / sizeof (data[0]);
    ADD_TESTS(ntests);

    for (int i = 0; i < ntests; ++i) {
        OctetIt first(iter_cast<OctetIt>(data[i].text));
        OctetIt last(iter_cast<OctetIt>(data[i].text) + data[i].len);
        pfs::unicode::u8_input_iterator<OctetIt> inp_first(first, last);
        pfs::unicode::u8_input_iterator<OctetIt> inp_last(last);

        pfs::unicode::char_t uc(0);
        std::string result;

        while (inp_first != inp_last) {
            uc = *inp_first++;
            pfs::unicode::u8_output_iterator<pfs::back_insert_iterator<std::string> > out(pfs::back_inserter(result));
            *out++ = uc;
        }

        std::ostringstream desc;

        desc << "Decode/Encode to UTF-8 using `" << itertype << "' as pointer. String `"
                << data[i].name << "'.";
        TEST_OK2(result == std::string(reinterpret_cast<char *> (data[i].text), data[i].len)
                , desc.str().c_str());
    }
}

void test_encode ()
{
    __test_encode_obsolete<unsigned char *>("unsigned char *");
    __test_encode_obsolete<const unsigned char *>("const unsigned char *");
    __test_encode_obsolete<char *>("char *");
    __test_encode_obsolete<const char *>("const char *");
    __test_encode_obsolete<std::string::iterator>("std::string::iterator");
    __test_encode_obsolete<std::string::const_iterator>("std::string::const_iterator");

    __test_encode<unsigned char *>("unsigned char *");
    __test_encode<const unsigned char *>("const unsigned char *");
    __test_encode<char *>("char *");
    __test_encode<const char *>("const char *");
    __test_encode<std::string::iterator>("std::string::iterator");
    __test_encode<std::string::const_iterator>("std::string::const_iterator");
}
