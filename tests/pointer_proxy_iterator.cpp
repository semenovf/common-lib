////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 Vladislav Trifochkin
//
// This file is part of [common-lib](https://github.com/semenovf/common-lib) library.
//
// Changelog:
//      2019.12.16 Initial version (inspired from https://github.com/semenovf/pfs)
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/iterator.hpp"
#include <cstring>

TEST_CASE("Pointer proxy iterator") {
    char const * sample = "Hello!";
    pfs::pointer_proxy_iterator<char const> first(sample);
    pfs::pointer_proxy_iterator<char const> last(sample + std::strlen(sample));

    {
        auto it = first;
        CHECK(std::distance(it, last) == std::strlen(sample));
    }

    // Test postfix increment
    {
        auto it = first;

        CHECK((*it++) == 'H');
        CHECK((*it++) == 'e');
        CHECK((*it++) == 'l');
        CHECK((*it++) == 'l');
        CHECK((*it++) == 'o');
        CHECK((*it++) == '!');
        CHECK(it == last);
    }

    // Test prefix increment
    {
        auto it = first;

        CHECK((*it)   == 'H');
        CHECK((*++it) == 'e');
        CHECK((*++it) == 'l');
        CHECK((*++it) == 'l');
        CHECK((*++it) == 'o');
        CHECK((*++it) == '!');
        CHECK(++it == last);
    }

    // Test postfix decrement
    {
        auto it = last;

        it--;
        CHECK((*it--) == '!');
        CHECK((*it--) == 'o');
        CHECK((*it--) == 'l');
        CHECK((*it--) == 'l');
        CHECK((*it--) == 'e');
        CHECK((*it--) == 'H');
        CHECK(++it == first);
    }

    // Test prefix decrement
    {
        auto it = last;

        CHECK((*--it) == '!');
        CHECK((*--it) == 'o');
        CHECK((*--it) == 'l');
        CHECK((*--it) == 'l');
        CHECK((*--it) == 'e');
        CHECK((*--it) == 'H');
        CHECK(it == first);
    }

    // Test equality
    {
        auto it1 = first;
        auto it2 = first;

        CHECK(it1 == first);
        CHECK(it2 == first);
        CHECK(it1 == it2++);
        CHECK(it1 != it2);
        CHECK(it2 != first);
    }

    // Test subscript
    {
        auto it = first;

        CHECK(it[0] == 'H');
        CHECK(it[1] == 'e');
        CHECK(it[2] == 'l');
        CHECK(it[3] == 'l');
        CHECK(it[4] == 'o');
        CHECK(it[5] == '!');
    }

    // Test +=, -=
    {
        auto it = first;

        it += 0;
        CHECK(*it == 'H');

        it -= 0;
        CHECK(*it == 'H');

        it += 1;
        CHECK(*it == 'e');

        it -= 1;
        CHECK(*it == 'H');

        it += 2;
        CHECK(*it == 'l');

        it -= 2;
        CHECK(*it == 'H');

        it += 3;
        CHECK(*it == 'l');

        it -= 3;
        CHECK(*it == 'H');

        it += 4;
        CHECK(*it == 'o');

        it -= 4;
        CHECK(*it == 'H');

        it += 5;
        CHECK(*it == '!');

        it -= 5;
        CHECK(*it == 'H');

    }

    {
        auto it = last;

        it -= std::distance(first, last);

        CHECK(*(it + 0) == 'H');
        CHECK(*(it + 1) == 'e');
        CHECK(*(it + 2) == 'l');
        CHECK(*(it + 3) == 'l');
        CHECK(*(it + 4) == 'o');
        CHECK(*(it + 5) == '!');

        it += std::distance(first, last);

        CHECK(*(it - 1) == '!');
        CHECK(*(it - 2) == 'o');
        CHECK(*(it - 3) == 'l');
        CHECK(*(it - 4) == 'l');
        CHECK(*(it - 5) == 'e');
        CHECK(*(it - 6) == 'H');
    }

    // Test difference
    {
        auto it1 = first;
        auto it2 = last;

        CHECK(it2 - it1 == std::strlen(sample));
        CHECK(it2 - it1 == std::distance(first, last));
        CHECK(-(it1 - it2) == std::strlen(sample));
        CHECK(-(it1 - it2) == std::distance(first, last));
        CHECK(it1 - it2 == std::distance(last, first));
    }

    // Test compare
    {
        auto it1 = first;
        auto it2 = first;
        auto it3 = last;

        CHECK(it1 == it2);
        CHECK(it1 != it3);
        CHECK(it1 <= it2);
        CHECK(it1 <= it3);
        CHECK(it1 < it3);
        CHECK(it2 >= it1);
        CHECK(it3 >= it1);
        CHECK(it3 > it1);
    }
}
