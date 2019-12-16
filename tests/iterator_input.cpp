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

struct A
{
    int x;
};

struct input_iterator : public pfs::iterator_facade<
          pfs::input_iterator_tag
        , input_iterator
        , A, A *, A &>
{
    A * _p;

    input_iterator (input_iterator const & rhs)
        : _p(rhs._p)
    {}

    input_iterator (A * p)
        : _p(p)
    {}

    static reference ref (input_iterator & it)
    {
        return *it._p;
    }

    static pointer ptr (input_iterator & it)
    {
        return it._p;
    }

    static void increment (input_iterator & it, difference_type)
    {
        ++it._p;
    }

    static bool equals (input_iterator const & it1, input_iterator const & it2)
    {
        return it1._p == it2._p;
    }
};

TEST_CASE("Input Iterator") {

    A data[] = { {1}, {2}, {3}, {4} };

    {
        input_iterator it(data);

        CHECK((*it++).x == 1);
        CHECK((*it++).x == 2);
        CHECK((*it++).x == 3);
        CHECK((*it++).x == 4);
    }

    // Test prefix increment
    {
        input_iterator it(data);
        CHECK((*it).x == 1);
        CHECK((*++it).x == 2);
        CHECK((*++it).x == 3);
        CHECK((*++it).x == 4);
    }

    // Test equality and `->` operator
    {
        input_iterator it1(data);
        input_iterator it2(data);

        CHECK(it1 == it2++);
        CHECK(it1 != it2);

        CHECK(it1->x == 1);
        CHECK(it2->x == 2);
    }
}
