////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020-2023 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2023.04.13 Initial version.
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/fmt.hpp"
#include "pfs/unicode/utf8_iterator.hpp"
#include "pfs/unicode/search.hpp"

TEST_CASE("iterate") {
    using utf8_input_iterator = pfs::unicode::utf8_input_iterator<char const *>;

    {
        char const * s = "абв";

        auto pos = utf8_input_iterator::begin(s, s + std::strlen(s));

        CHECK_EQ(*pos++, pfs::unicode::char_t{0x0430});
        CHECK_EQ(*pos++, pfs::unicode::char_t{0x0431});
        CHECK_EQ(*pos++, pfs::unicode::char_t{0x0432});

        CHECK_EQ(pos, pos.end());
        CHECK_THROWS(++pos);
    }

    {
        char const * s = "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЬЫЪЭЮЯабвгдеёжзийклмнопрстуфхцчшщьыъэюя";

        auto pos = utf8_input_iterator::begin(s, s + std::strlen(s));
        auto end = pos.end();

        int count = 0;

        while (pos != end) {
            pos++;
            ++count;
        }

        CHECK_EQ(count, 66);
    }
}

TEST_CASE("distance") {
    using utf8_input_iterator = pfs::unicode::utf8_input_iterator<char const *>;

    {
        char const * s = "абв";
        auto pos = utf8_input_iterator::begin(s, s + std::strlen(s));
        CHECK_EQ(std::distance(pos, pos.end()), 3);
    }

    {
        char const * s = "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЬЫЪЭЮЯабвгдеёжзийклмнопрстуфхцчшщьыъэюя";
        auto pos = utf8_input_iterator::begin(s, s + std::strlen(s));
        CHECK_EQ(std::distance(pos, pos.end()), 66);
    }

    {
        using utf8_it1_t = pfs::unicode::utf8_input_iterator<char const *>;
        using utf8_it2_t = pfs::unicode::utf8_input_iterator<std::string::const_iterator>;

        char const * s = "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЬЫЪЭЮЯабвгдеёжзийклмнопрстуфхцчшщьыъэюя";
        std::string str{s};

        auto pos1 = utf8_it1_t::begin(s, s + std::strlen(s));
        auto res = utf8_it1_t::distance_unsafe(pos1, pos1.end());

        CHECK_EQ(res.first, 66);      // Number of code points
        CHECK_EQ(res.second, 2 * 66); // Number of code units

        auto pos2 = utf8_it2_t::begin(str.begin(), str.end());
        res = utf8_it2_t::distance_unsafe(pos2, pos2.end());

        CHECK_EQ(res.first, 66);      // Number of code points
        CHECK_EQ(res.second, 2 * 66); // Number of code units
    }
}

TEST_CASE("advance") {
    using utf8_input_iterator = pfs::unicode::utf8_input_iterator<char const *>;

    {
        char const * s = "абв";
        auto pos = utf8_input_iterator::begin(s, s + std::strlen(s));
        auto end = pos.end();
        std::advance(pos, 3);
        CHECK_EQ(pos, end);
    }

    {
        char const * s = "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЬЫЪЭЮЯабвгдеёжзийклмнопрстуфхцчшщьыъэюя";

        auto pos = utf8_input_iterator::begin(s, s + std::strlen(s));
        auto end = pos.end();
        std::advance(pos, 66);
        CHECK_EQ(pos, end);

        auto pos1 = utf8_input_iterator::begin(s, s + std::strlen(s));
        auto end1 = pos1.end();
        utf8_input_iterator::advance_unsafe(pos1, 66);
        CHECK_EQ(pos1, end1);
    }
}

TEST_CASE("search") {
    using utf8_input_iterator = pfs::unicode::utf8_input_iterator<char const *>;

    {
        char const * haystack = "Lorem ipsum dolor sit amet. Vis lorem."
            " His an LoReM, quod altera loreM. Ei has LOREM.";

        int count = 0;
        char const * needle = "loRem";
        bool ignore_case = true;

        auto first = utf8_input_iterator::begin(haystack, haystack + std::strlen(haystack));
        auto last = first.end();
        auto s_first = utf8_input_iterator::begin(needle, needle + std::strlen(needle));

        pfs::unicode::search_all(first, last, s_first, s_first.end(), ignore_case
            , [first, last, & count] (pfs::unicode::match_item const & m) {
                std::string prefix (first.base(), first.base() + m.cu_first);
                std::string substr (first.base() + m.cu_first, first.base() + m.cu_last);
                std::string suffix (first.base() + m.cu_last, last.base());

                fmt::print("{}[{}]{}: {}-{},{}-{}\n", prefix, substr, suffix
                    , m.cp_first, m.cp_last, m.cu_first, m.cu_last);

                count++;

                switch (count) {
                    case 1:
                        CHECK_EQ(m.cp_first, 0);
                        CHECK_EQ(m.cp_last , 5);
                        CHECK_EQ(m.cu_first, 0);
                        CHECK_EQ(m.cu_last , 5);
                        break;
                    case 2:
                        CHECK_EQ(m.cp_first, 32);
                        CHECK_EQ(m.cp_last , 37);
                        CHECK_EQ(m.cu_first, 32);
                        CHECK_EQ(m.cu_last , 37);
                        break;
                    case 3:
                        CHECK_EQ(m.cp_first, 46);
                        CHECK_EQ(m.cp_last , 51);
                        CHECK_EQ(m.cu_first, 46);
                        CHECK_EQ(m.cu_last , 51);
                        break;
                    case 4:
                        CHECK_EQ(m.cp_first, 65);
                        CHECK_EQ(m.cp_last , 70);
                        CHECK_EQ(m.cu_first, 65);
                        CHECK_EQ(m.cu_last , 70);
                        break;
                    case 5:
                        CHECK_EQ(m.cp_first, 79);
                        CHECK_EQ(m.cp_last , 84);
                        CHECK_EQ(m.cu_first, 79);
                        CHECK_EQ(m.cu_last , 84);
                        break;
                }
        });

        CHECK_MESSAGE(count == 5, "Wrong number of occurances");
    }


    {
        char const * haystack = "Лорем ипсум долор сит амет. Вис лорем."
            " Хис ан ЛоРеМ, куад алтера лореМ. Еи хас ЛОРЕМ.";

        int count = 0;
        char const * needle = "лоРем";
        bool ignore_case = true;

        auto first = utf8_input_iterator::begin(haystack, haystack + std::strlen(haystack));
        auto last = first.end();
        auto s_first = utf8_input_iterator::begin(needle, needle + std::strlen(needle));

        pfs::unicode::search_all(first, last, s_first, s_first.end(), ignore_case
            , [first, last, & count] (pfs::unicode::match_item const & m) {
                std::string prefix (first.base(), first.base() + m.cu_first);
                std::string substr (first.base() + m.cu_first, first.base() + m.cu_last);
                std::string suffix (first.base() + m.cu_last, last.base());

                fmt::print("{}[{}]{}: {}-{},{}-{}\n", prefix, substr, suffix
                    , m.cp_first, m.cp_last, m.cu_first, m.cu_last);

                count++;

                switch (count) {
                    case 1:
                        CHECK_EQ(m.cp_first, 0);
                        CHECK_EQ(m.cp_last , 5);
                        CHECK_EQ(m.cu_first, 0);
                        CHECK_EQ(m.cu_last , 10);
                        break;
                    case 2:
                        CHECK_EQ(m.cp_first, 32);
                        CHECK_EQ(m.cp_last , 37);
                        CHECK_EQ(m.cu_first, 57);
                        CHECK_EQ(m.cu_last , 67);
                        break;
                    case 3:
                        CHECK_EQ(m.cp_first, 46);
                        CHECK_EQ(m.cp_last , 51);
                        CHECK_EQ(m.cu_first, 81);
                        CHECK_EQ(m.cu_last , 91);
                        break;
                    case 4:
                        CHECK_EQ(m.cp_first, 65);
                        CHECK_EQ(m.cp_last , 70);
                        CHECK_EQ(m.cu_first, 115);
                        CHECK_EQ(m.cu_last , 125);
                        break;
                    case 5:
                        CHECK_EQ(m.cp_first, 79);
                        CHECK_EQ(m.cp_last , 84);
                        CHECK_EQ(m.cu_first, 139);
                        CHECK_EQ(m.cu_last , 149);
                        break;
                }
        });

        CHECK_MESSAGE(count == 5, "Wrong number of occurances");
    }

    {
        using char_t = pfs::unicode::char_t;
        char const * haystack = "<>w<>";
        char const * needle = "w";
        auto first = utf8_input_iterator::begin(haystack, haystack + std::strlen(haystack));
        auto last = first.end();
        auto s_first = utf8_input_iterator::begin(needle, needle + std::strlen(needle));

        auto pos = pfs::unicode::search(first, last, s_first, s_first.end()
            , '<', '>', [] (char_t a, char_t b) { return a == b; });

        CHECK(pos != last);
    }

    {
        char const * haystack = "<span Лорем>Лорем</Лорем span> ипсум долор сит амет. "
            "<p лорем>Вис лорем.</p лорем>"
            "<div>Хис ан ЛоРеМ, куад алтера лореМ. Еи хас ЛОРЕМ.</ЛОРЕМ div>";

        int count = 0;
        char const * needle = "лоРем";
        bool ignore_case = true;

        auto first = utf8_input_iterator::begin(haystack, haystack + std::strlen(haystack));
        auto last = first.end();
        auto s_first = utf8_input_iterator::begin(needle, needle + std::strlen(needle));

        pfs::unicode::search_all(first, last, s_first, s_first.end(), ignore_case
            , '<', '>'
            , [first, last, & count] (pfs::unicode::match_item const & m) {
                std::string prefix (first.base(), first.base() + m.cu_first);
                std::string substr (first.base() + m.cu_first, first.base() + m.cu_last);
                std::string suffix (first.base() + m.cu_last, last.base());

                fmt::print("{}[{}]{}: {}-{},{}-{}\n", prefix, substr, suffix
                    , m.cp_first, m.cp_last, m.cu_first, m.cu_last);

                count++;

                switch (count) {
                    case 1:
                        CHECK_EQ(m.cp_first, 12);
                        CHECK_EQ(m.cp_last , 17);
                        CHECK_EQ(m.cu_first, 17);
                        CHECK_EQ(m.cu_last , 27);
                        break;
                    case 2:
                        CHECK_EQ(m.cp_first,  66);
                        CHECK_EQ(m.cp_last ,  71);
                        CHECK_EQ(m.cu_first, 106);
                        CHECK_EQ(m.cu_last , 116);
                        break;
                    case 3:
                        CHECK_EQ(m.cp_first,  94);
                        CHECK_EQ(m.cp_last ,  99);
                        CHECK_EQ(m.cu_first, 149);
                        CHECK_EQ(m.cu_last , 159);
                        break;
                    case 4:
                        CHECK_EQ(m.cp_first, 113);
                        CHECK_EQ(m.cp_last , 118);
                        CHECK_EQ(m.cu_first, 183);
                        CHECK_EQ(m.cu_last , 193);
                        break;
                    case 5:
                        CHECK_EQ(m.cp_first, 127);
                        CHECK_EQ(m.cp_last , 132);
                        CHECK_EQ(m.cu_first, 207);
                        CHECK_EQ(m.cu_last , 217);
                        break;
                }
        });

        CHECK_MESSAGE(count == 5, "Wrong number of occurances");
    }
}
