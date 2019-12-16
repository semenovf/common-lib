////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 Vladislav Trifochkin
//
// This file is part of [pfs-common](https://github.com/semenovf/pfs-common) library.
//
// Changelog:
//      2019.12.16 Initial version (inspired from https://github.com/semenovf/pfs)
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/iterator.hpp"

struct D
{
    int x;
};

struct bidirectional_iterator : public pfs::iterator_facade<
          pfs::bidirectional_iterator_tag
        , bidirectional_iterator
        , D, D *, D &>
{
    D * _p;

    bidirectional_iterator (D * p)
        : _p(p)
    {}

    static reference ref (bidirectional_iterator & it)
    {
        return *it._p;
    }

    static pointer ptr (bidirectional_iterator & it)
    {
        return it._p;
    }

    static void increment (bidirectional_iterator & it, difference_type)
    {
        ++it._p;
    }

    static bool equals (bidirectional_iterator const & it1, bidirectional_iterator const & it2)
    {
        return it1._p == it2._p;
    }

    static void decrement (bidirectional_iterator & it, difference_type)
    {
        --it._p;
    }
};

TEST_CASE("Bidirectional Iterator") {
    D data[] = { {1}, {2}, {3}, {4} };

    // Test postfix increment
    {
        bidirectional_iterator it(data);

        CHECK((*it++).x == 1);
        CHECK((*it++).x == 2);
        CHECK((*it++).x == 3);
        CHECK((*it++).x == 4);
    }

    // Test prefix increment
    {
        bidirectional_iterator it(data);
        CHECK((*it).x == 1);
        CHECK((*++it).x == 2);
        CHECK((*++it).x == 3);
        CHECK((*++it).x == 4);
    }

    // Test postfix decrement
    {
        bidirectional_iterator it(data + sizeof(data)/sizeof(data[0]));

        it--;
        CHECK((*it--).x == 4);
        CHECK((*it--).x == 3);
        CHECK((*it--).x == 2);
        CHECK((*it--).x == 1);
    }

    // Test prefix decrement
    {
        bidirectional_iterator it(data + sizeof(data)/sizeof(data[0]));

        CHECK((*--it).x == 4);
        CHECK((*--it).x == 3);
        CHECK((*--it).x == 2);
        CHECK((*--it).x == 1);
    }

    // Test prefixed increment, equality and `->` operator
    {
        bidirectional_iterator it1(data);
        bidirectional_iterator it2(data);

        CHECK(++it1 == ++it2);
        CHECK(it1 == it2);

        CHECK(it1->x == 2);
        CHECK(it2->x == 2);
    }

    // Test postfix increment, equality and `->` operator
    {
        bidirectional_iterator it1(data);
        bidirectional_iterator it2(data);

        CHECK(it1 == it2++);
        CHECK(it1 != it2);

        CHECK(it1->x == 1);
        CHECK(it2->x == 2);
    }
}
