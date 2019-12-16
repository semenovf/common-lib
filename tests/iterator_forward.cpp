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

struct C
{
    int x;
};

struct forward_iterator : public pfs::iterator_facade<
          pfs::forward_iterator_tag
        , forward_iterator
        , C, C *, C &>
{
    C * _p;

//    forward_iterator (input_iterator const & rhs)
//        : _p(rhs._p)
//    {}

    forward_iterator (C * p)
        : _p(p)
    {}

    static reference ref (forward_iterator & it)
    {
        return *it._p;
    }

    static pointer ptr (forward_iterator & it)
    {
        return it._p;
    }

    static void increment (forward_iterator & it, difference_type)
    {
        ++it._p;
    }

    static bool equals (forward_iterator const & it1, forward_iterator const & it2)
    {
        return it1._p == it2._p;
    }
};

TEST_CASE("Forward Iterator") {

    C data[] = { {1}, {2}, {3}, {4} };

    {
        forward_iterator it(data);

        CHECK((*it++).x == 1);
        CHECK((*it++).x == 2);
        CHECK((*it++).x == 3);
        CHECK((*it++).x == 4);
    }

    // Test prefix increment
    {
        forward_iterator it(data);
        CHECK((*it).x == 1);
        CHECK((*++it).x == 2);
        CHECK((*++it).x == 3);
        CHECK((*++it).x == 4);
    }

    // Test equality and `->` operator
    {
        forward_iterator it1(data);
        forward_iterator it2(data);

        CHECK(it1 == it2++);
        CHECK(it1 != it2);

        CHECK(it1->x == 1);
        CHECK(it2->x == 2);
    }
}
