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

    forward_iterator (C * p)
        : _p(p)
    {}

    reference ref ()
    {
        return *_p;
    }

    pointer ptr ()
    {
        return _p;
    }

    void increment (difference_type)
    {
        ++_p;
    }

    bool equals (forward_iterator const & rhs) const
    {
        return _p == rhs._p;
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
