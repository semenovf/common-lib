#include <pfs/test.hpp>
#include <pfs/unicode/unicode_iterator.hpp>
#include <pfs/unicode/u8_iterator.hpp>
#include <string>
#include <sstream>
#include <fstream>
#include "test_data.hpp"

namespace pfs {
namespace unicode {

template <>
struct unicode_iterator_traits<std::string::iterator>
    :  unicode_iterator_limits<std::string::iterator, char *>
{
    typedef utf8_iterator<std::string::iterator> iterator;
    typedef u8_output_iterator<std::string> output_iterator;
    typedef u8_input_iterator<std::string::iterator> input_iterator;
};

template <>
struct unicode_iterator_traits<std::string::const_iterator>
    :  unicode_iterator_limits<std::string::const_iterator, char const *>
{
    typedef utf8_iterator<std::string::const_iterator> iterator;
    typedef u8_input_iterator<std::string::const_iterator> input_iterator;
};

}} // pfs::unicode


template <typename OctetIt>
void __test_input (char const * itertype)
{
    typedef typename pfs::unicode::u8_input_iterator<OctetIt> utf8_input_iterator;

    int ntests = sizeof(data)/sizeof(data[0]);
    ADD_TESTS(ntests);

    for (int i = 0; i < ntests; ++i) {
        OctetIt first = iter_cast<OctetIt>(data[i].text);
        utf8_input_iterator it(first, iter_cast<OctetIt>(data[i].text) + data[i].len);
        utf8_input_iterator it_end(iter_cast<OctetIt>(data[i].text) + data[i].len);

        size_t count = 0;

        while (it != it_end) {
            ++it;
            ++count;
        }

        std::ostringstream desc;

        desc << "Iterate using pfs::unicode::utf8_input_iterator<" << itertype << ">."
                << " String `" << data[i].name << "'."
                << " Number of unicode chars " << count << ", expected " << data[i].nchars;
        TEST_OK2(count == data[i].nchars, desc.str().c_str());
    }
    }

template <typename OctetIt>
void __test_forward (const char * itertype)
{
    typedef typename pfs::unicode::utf8_iterator<OctetIt> utf8_iterator;

    int ntests = sizeof(data)/sizeof(data[0]);
    ADD_TESTS(ntests);

    for (int i = 0; i < ntests; ++i) {
        utf8_iterator it(iter_cast<OctetIt>(data[i].text));
        utf8_iterator it_end(iter_cast<OctetIt>(data[i].text) + data[i].len);
        size_t count = 0;

        while (it < it_end) {
            ++it;
            ++count;
        }

        std::ostringstream desc;

        desc << "Forward iterate using pfs::utf8::iterator<" << itertype << ">."
                << " String `" << data[i].name << "'."
                << " Number of unicode chars " << count << ", expected " << data[i].nchars;
        TEST_OK2(count == data[i].nchars, desc.str().c_str());
    }
}

template <typename OctetIt>
void __test_backward (const char * itertype)
{
    typedef typename pfs::unicode::utf8_iterator<OctetIt> utf8_iterator;

    int ntests = sizeof(data)/sizeof(data[0]);
    ADD_TESTS(ntests);

    for (int i = 0; i < ntests; ++i) {
        utf8_iterator it_begin(iter_cast<OctetIt>(data[i].text));
        utf8_iterator it(iter_cast<OctetIt>(data[i].text) + data[i].len);
        size_t count = 0;

        while (it-- > it_begin) {
            ++count;
        }

        std::ostringstream desc;

        desc << "Reverse iterate using pfs::utf8::iterator<" << itertype << ">. String `"
                << data[i].name
                << "'. Number of unicode chars "
                << count
                << ", expected "
                << data[i].nchars;
        TEST_OK2(count == data[i].nchars, desc.str().c_str());
    }
}

template <typename OctetIt>
void __test_std_advance_forward(const char * itertype)
{
    typedef typename pfs::unicode::utf8_iterator<OctetIt> utf8_iterator;

    int ntests = sizeof(data)/sizeof(data[0]);
    ADD_TESTS(ntests * 2);

    for (int i = 0; i < ntests; ++i) {
        utf8_iterator it_begin(iter_cast<OctetIt>(data[i].text));
        utf8_iterator it_end(iter_cast<OctetIt>(data[i].text) + data[i].len);
        size_t count = 0;

        utf8_iterator it = it_begin;

        while (it < it_end) {
            std::advance(it, 1);
            ++count;
        }

        std::ostringstream desc;

        desc << "Forward iterate using std::advance(pfs::utf8::iterator<" << itertype << "> &). String `"
                << data[i].name
                << "'. Number of unicode chars "
                << count
                << ", expected "
                << data[i].nchars;
        TEST_OK2(count == data[i].nchars, desc.str().c_str());

        it = it_begin;
        std::advance(it, count);
        std::ostringstream desc1;
        desc1 << "Jump to the end by std::advance(pfs::utf8::iterator<" << itertype << "> &)";
        TEST_OK2(it == it_end, desc1.str().c_str());
    }
}

template <typename OctetIt>
void __test_std_advance_backward (const char * itertype)
{
    typedef typename pfs::unicode::utf8_iterator<OctetIt> utf8_iterator;

    int ntests = sizeof(data)/sizeof(data[0]);
    ADD_TESTS(ntests * 2);

    for (int i = 0; i < ntests; ++i) {
        utf8_iterator it_begin(iter_cast<OctetIt>(data[i].text));
        utf8_iterator it_end(iter_cast<OctetIt>(data[i].text) + data[i].len);
        size_t count = 0;

        utf8_iterator it = it_end;

        while (it > it_begin) {
            std::advance(it, -1);
            ++count;
        }

        std::ostringstream desc;

        desc << "Reverse iterate using std::advance(pfs::utf8::iterator<" << itertype << "> &). String `"
                << data[i].name
                << "'. Number of unicode chars "
                << count
                << ", expected "
                << data[i].nchars;
        TEST_OK2(count == data[i].nchars, desc.str().c_str());

        it = it_end;
        std::advance(it, -count);
        std::ostringstream desc1;
        desc1 << "Jump to the begin by std::advance(pfs::utf8::iterator<" << itertype << "> &)";
        TEST_OK2(it == it_begin, desc1.str().c_str());
    }
}

template <typename OctetIt>
void __test_std_reverse_iterator(const char * itertype)
{
    typedef typename pfs::unicode::utf8_iterator<OctetIt> utf8_iterator;

    int ntests = sizeof(data)/sizeof(data[0]);
    ADD_TESTS(ntests);

    typedef std::reverse_iterator<utf8_iterator> utf8_reverse_iterator;

    for (int i = 0; i < ntests; ++i) {
        utf8_reverse_iterator it(utf8_iterator(iter_cast<OctetIt>(data[i].text) + data[i].len));
        utf8_reverse_iterator it_end(utf8_iterator(iter_cast<OctetIt>(data[i].text)));
        size_t count = 0;

        while (it < it_end) {
            ++it;
            ++count;
        }

        std::ostringstream desc;

        desc << "Iterate forward using std::reverse_iterator<pfs::utf8::iterator<" << itertype << ">>. "
                << " String `" << data[i].name << "'. "
                << " Number of unicode chars " << count << ", expected " << data[i].nchars;
        TEST_OK2(count == data[i].nchars, desc.str().c_str());
    }
}

void test_iterator ()
{
    __test_input<unsigned char *>("unsigned char *");
    __test_input<const unsigned char *>("const unsigned char *");
    __test_input<char *>("char *");
    __test_input<const char *>("const char *");
    __test_input<std::string::iterator>("std::string::iterator");
    __test_input<std::string::const_iterator>("std::string::const_iterator");

    __test_forward<unsigned char *>("unsigned char *");
    __test_forward<const unsigned char *>("const unsigned char *");
    __test_forward<char *>("char *");
    __test_forward<const char *>("const char *");
    __test_forward<std::string::iterator>("std::string::iterator");
    __test_forward<std::string::const_iterator>("std::string::const_iterator");

    __test_backward<unsigned char *>("unsigned char *");
    __test_backward<const unsigned char *>("const unsigned char *");
    __test_backward<char *>("char *");
    __test_backward<const char *>("const char *");
    __test_backward<std::string::iterator>("std::string::iterator");
    __test_backward<std::string::const_iterator>("std::string::const_iterator");

    __test_std_advance_forward<unsigned char *>("unsigned char *");
    __test_std_advance_forward<const unsigned char *>("const unsigned char *");
    __test_std_advance_forward<char *>("char *");
    __test_std_advance_forward<const char *>("const char *");
    __test_std_advance_forward<std::string::iterator>("std::string::iterator");
    __test_std_advance_forward<std::string::const_iterator>("std::string::const_iterator");

    __test_std_advance_backward<unsigned char *>("unsigned char *");
    __test_std_advance_backward<const unsigned char *>("const unsigned char *");
    __test_std_advance_backward<char *>("char *");
    __test_std_advance_backward<const char *>("const char *");
    __test_std_advance_backward<std::string::iterator>("std::string::iterator");
    __test_std_advance_backward<std::string::const_iterator>("std::string::const_iterator");

    __test_std_reverse_iterator<unsigned char *>("unsigned char *");
    __test_std_reverse_iterator<const unsigned char *>("const unsigned char *");
    __test_std_reverse_iterator<char *>("char *");
    __test_std_reverse_iterator<const char *>("const char *");
    __test_std_reverse_iterator<std::string::iterator>("std::string::iterator");
    __test_std_reverse_iterator<std::string::const_iterator>("std::string::const_iterator");
}
