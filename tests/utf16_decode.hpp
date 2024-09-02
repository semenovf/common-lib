////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020-2023 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2020.11.08 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/fmt.hpp"
#include <pfs/unicode/utf16_iterator.hpp>
#include <cstring>
#include <fstream>
#include <iterator>
#include <string>
#include "unicode/test_data.hpp"

template <typename HextetIt, typename Utf18ByteSwap>
bool decode (char const * itertype)
{
    int ntests = sizeof(data) / sizeof(data[0]);
    bool result {true};

    for (int i = 0; i < ntests; ++i) {
        REQUIRE_MESSAGE(data[i].len % 2 == 0, "Test text length must be even");

        // Convert bytes sequence into word sequence according to native byte order
        std::vector<std::uint16_t> d;

        for (decltype(data[i].len) j = 0, count = data[i].len; j < count; j += 2) {
            std::uint16_t value = *reinterpret_cast<std::uint16_t const *>(& data[i].text[j]);
            d.push_back(value);
        }

        HextetIt first(iter_cast<HextetIt>(d.data()));
        HextetIt last(first + d.size());

        size_t count = 0;
        pfs::unicode::char_t uc = 0;
        auto inp_first = pfs::unicode::utf16_iterator<HextetIt, Utf18ByteSwap>::begin(first, last);
        auto inp_last  = pfs::unicode::utf16_iterator<HextetIt, Utf18ByteSwap>::end(last);

        while (inp_first != inp_last) {
            uc = *inp_first++;
            ++count;
        }

        CHECK_MESSAGE(count == data[i].nchars
            , fmt::format("Decode UTF-16 using `{}' as pointer. String `{}`"
                , itertype, data[i].name));

        if (count != data[i].nchars) {
            auto desc = fmt::format("Decode UTF-16 using `{}' as pointer."
                " String `{}'. Number of unicode chars {}, expected {}"
                , itertype, data[i].name, count, data[i].nchars);
            MESSAGE(desc);
            result = false;
        }
    }

    return result;
}

// FIXME Need right specializations of below structs to work with
// std::basic_ifstream<std::uint16_t> and std::istreambuf_iterator<std::uint16_t>
// in decode_files() function

// namespace std {
//
// template <>
// struct char_traits<std::uint16_t>
// {
//     typedef std::uint16_t char_type;
//
//     //typedef uint_least_16_t int_type;
//     typedef std::uint32_t int_type;
//
//     typedef streamoff off_type;
//     typedef std::streampos pos_type;
//     typedef mbstate_t state_type;
//
//     static void assign (char_type & c1, char_type const & c2)
//     {
//         c1 = c2;
//     }
//
//     // static bool eq(const char_type& c1, const char_type& c2);
//     // static bool lt(const char_type& c1, const char_type& c2);
//     // static int compare(const char_type* s1, const char_type* s2, size_t n);
//     // static size_t length(const char_type* s);
//     // static const char_type* find(const char_type* s, size_t n,
//     // const char_type& a);
//     // static char_type* move(char_type* s1, const char_type* s2, size_t n);
//
//     static char_type * copy (char_type * s1, const char_type * s2, size_t n)
//     {
//         if (n == 0)
//             return s1;
//         return static_cast<char_type *>(std::memcpy(s1, s2, n * sizeof(char_type)));
//     }
//
//     // static char_type* assign(char_type* s, size_t n, char_type a);
//
//     static int_type not_eof (int_type const & c)
//     {
//         return (c == eof()) ? 0 : c;
//     }
//
//     static char_type to_char_type (int_type const & c)
//     {
//         return static_cast<char_type>(c);
//     }
//
//     static int_type to_int_type (char_type const & c)
//     {
//         return static_cast<int_type>(c);
//     }
//
//     static bool eq_int_type (int_type const & c1, int_type const & c2)
//     {
//         return c1 == c2;
//     }
//
//     static int_type eof ()
//     {
//         return static_cast<int_type>(0xffffffffu);
//     }
// };

// template <>
// struct codecvt<std::uint16_t, std::uint16_t, std::mbstate_t>
// {
//     typedef std::uint16_t char_type;
//     typedef std::uint32_t int_type;
//     typedef std::streamoff off_type;
//     typedef std::streampos pos_type;
//     typedef std::mbstate_t state_type;
// };

// } // namespace std

template <typename Utf18ByteSwap>
bool decode_files ()
{
    int ntests = sizeof (data) / sizeof (data[0]);
    bool result {true};

    for (int i = 0; i < ntests; ++i) {
        {
            std::basic_ifstream<std::uint16_t> ifs(data[i].filename, std::ios::binary);

            REQUIRE_MESSAGE(ifs.is_open()
                , fmt::format("Open file `{}` failure\n", data[i].filename));

            std::istreambuf_iterator<std::uint16_t> first(ifs);
            std::istreambuf_iterator<std::uint16_t> last;

            auto inp_first = pfs::unicode::utf16_iterator<std::istreambuf_iterator<std::uint16_t>, Utf18ByteSwap>::begin(first, last);
            auto inp_last  = pfs::unicode::utf16_iterator<std::istreambuf_iterator<std::uint16_t>, Utf18ByteSwap>::end(last);

            CHECK_EQ(std::distance(inp_first, inp_last), data[i].nchars);
        }

        {
            std::basic_ifstream<std::uint16_t> ifs(data[i].filename, std::ios::binary);

            REQUIRE_MESSAGE(ifs.is_open()
                , fmt::format("Open file `{}` failure\n", data[i].filename));

            std::istreambuf_iterator<std::uint16_t> first(ifs);
            std::istreambuf_iterator<std::uint16_t> last;

            size_t count = 0;
            auto inp_first = pfs::unicode::utf16_iterator<std::istreambuf_iterator<std::uint16_t>, Utf18ByteSwap>::begin(first, last);
            auto inp_last  = pfs::unicode::utf16_iterator<std::istreambuf_iterator<std::uint16_t>, Utf18ByteSwap>::end(last);

            while (inp_first != inp_last) {
                ++inp_first;
                ++count;
            }

            if (count != data[i].nchars) {
                auto msg = fmt::format("Decode UTF-16 using `istreambuf_iterator`"
                    " as pointer. String `{}'. Number of unicode chars {}, expected {}"
                    , data[i].name, count, data[i].nchars);
                MESSAGE(msg);
                result = false;
            }
        }
    }

    return result;
}
