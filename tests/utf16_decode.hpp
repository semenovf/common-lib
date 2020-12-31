////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of [pfs-common](https://github.com/semenovf/pfs-common) library.
//
// Changelog:
//      2020.11.08 Initial version
//
////////////////////////////////////////////////////////////////////////////////
#pragma once
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <pfs/endian.hpp>
#include <pfs/unicode/unicode_iterator.hpp>
#include <pfs/unicode/u16_iterator.hpp>
#include <fstream>
#include <iterator>
#include <string>
#include <sstream>
#include "unicode/test_data.hpp"

template <typename HextetInputIt>
bool decode (const char * itertype, bool is_little_endian)
{
    int ntests = sizeof(data) / sizeof(data[0]);
    bool result {true};

    for (int i = 0; i < ntests; ++i) {
        REQUIRE_MESSAGE(data[i].len % 2 == 0, "Test text length must be even");

        // Convert bytes sequence into word sequence according to native byte order
        std::vector<uint16_t> d;

        for (decltype(data[i].len) j = 0, count = data[i].len; j < count; j += 2) {

            // NOTE This conversion algoritm depends on source byte order
            if (is_little_endian) {
                uint16_t value = (data[i].text[j + 1] << 8) | data[i].text[j];
                d.push_back(value);
            } else {
                uint16_t value = (data[i].text[j] << 8) | data[i].text[j + 1];
                d.push_back(value);
            }
        }

        HextetInputIt first(iter_cast<HextetInputIt>(d.data()));
        HextetInputIt last(iter_cast<HextetInputIt>(d.data() + d.size()));

        size_t count = 0;
        pfs::unicode::char_t uc = 0;
        pfs::unicode::u16_input_iterator<HextetInputIt> inp_first(first, last, is_little_endian);
        pfs::unicode::u16_input_iterator<HextetInputIt> inp_last(last, is_little_endian);

        while (inp_first != inp_last) {
            uc = *inp_first++;
            ++count;
        }

        CHECK_MESSAGE(!inp_first.is_broken(), "Sequence is broken");

        std::ostringstream msg;
        msg << "Decode UTF-16 using `" << itertype << "' as pointer. String `"
                    << data[i].name << "`, endianess: " << (is_little_endian ? "little" : "big.");

        CHECK_MESSAGE(count == data[i].nchars, msg.str());

        if (count != data[i].nchars) {
            std::ostringstream desc;

            desc << "Decode UTF-16 using `" << itertype << "' as pointer. String `"
                    << data[i].name
                    << "`, endianess: " << (is_little_endian ? "little" : "big.")
                    << " Number of unicode chars "
                    << count
                    << ", expected "
                    << data[i].nchars;
            MESSAGE(desc.str());
            result = false;
        }
    }

    return result;
}

namespace std {

    // FIXME
// template<>
// struct char_traits<uint16_t>
// {
//     typedef uint16_t char_type;
//     typedef uint32_t int_type;
//     typedef std::streamoff off_type;
//     typedef std::streampos pos_type;
//     typedef std::mbstate_t state_type;
//
//     static void assign (char_type & dst, char_type const src)
//     {
//         dst = src;
//     }
//
//     static constexpr char_type to_char_type (int_type const & c) noexcept
//     {
//         return static_cast<char_type>(c);
//     }
//
//     static constexpr int_type to_int_type (char_type const & c) noexcept
//     {
//         return static_cast<int_type>(c);
//     }
//
//     static constexpr bool eq_int_type (int_type const & c1
//             , int_type const & c2) noexcept
//     {
//         return c1 == c2;
//     }
//
//     static char_type * copy (char_type * s1, char_type const * s2, size_t n)
//     {
//         if (n == 0)
//             return s1;
//         return static_cast<char_type *>(std::memcpy(s1, s2, n * sizeof(char_type)));
//     }
//
//     static constexpr int_type eof () noexcept
//     {
//         return static_cast<int_type>(0xffffffffu);
//     }
//
//     static constexpr int_type not_eof (int_type const & c) noexcept
//     {
//         return (c == eof()) ? 0 : c;
//     }
// };

template<>
struct ctype<uint16_t>
{
    static locale::id id;
};

locale::id ctype<uint16_t>::id;// = 1234567;

// ctype<wchar_t> ct;

// template<>
// struct codecvt<uint16_t, uint16_t, std::mbstate_t>
// {
//     typedef uint16_t char_type;
//     typedef uint32_t int_type;
//     typedef std::streamoff off_type;
//     typedef std::streampos pos_type;
//     typedef std::mbstate_t state_type;
// };

} // namespace std

bool decode_files (bool is_little_endian)
{
    int ntests = sizeof (data) / sizeof (data[0]);
    bool result {true};

    for (int i = 0; i < ntests; ++i) {
        //std::ifstream ifs(data[i].filename, std::ios::binary);
        std::basic_ifstream<uint16_t> ifs(data[i].filename, std::ios::binary);

        auto c = ifs.rdbuf()->sgetc();

        std::ostringstream msg;
        msg << "Open file `" << data[i].filename << "` failure\n";
        REQUIRE_MESSAGE(ifs.is_open(), msg.str());

        std::istreambuf_iterator<uint16_t> first(ifs);
        std::istreambuf_iterator<uint16_t> last;

        size_t count = 0;
        pfs::unicode::char_t uc = 0;
        pfs::unicode::u16_input_iterator<std::istreambuf_iterator<uint16_t>> inp_first(first, last, is_little_endian);
        pfs::unicode::u16_input_iterator<std::istreambuf_iterator<uint16_t>> inp_last(last, is_little_endian);

        while (inp_first != inp_last) {
            uc = *inp_first++;
            ++count;
        }

        if (count != data[i].nchars) {
            std::ostringstream msg;

            msg << "Decode UTF-16 using `istreambuf_iterator` as pointer. String `"
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
