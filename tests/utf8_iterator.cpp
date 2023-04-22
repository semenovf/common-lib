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
#include "pfs/unicode/utf8_iterator.hpp"

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

        auto res = utf8_it1_t::distance_unsafe(s, s + std::strlen(s));

        CHECK_EQ(res.first, 66);      // Number of code points
        CHECK_EQ(res.second, 2 * 66); // Number of code units

        res = utf8_it2_t::distance_unsafe(str.begin(), str.end());

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

        auto pos1 = s;
        utf8_input_iterator::advance_unsafe(pos1, 66);
        CHECK_EQ(pos1, s + std::strlen(s));
    }
}
