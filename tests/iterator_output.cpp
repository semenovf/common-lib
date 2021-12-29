////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2019.12.16 Initial version (inspired from https://github.com/semenovf/pfs)
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/iterator.hpp"

struct B
{
    int x;
};

struct output_iterator : public pfs::iterator_facade<
          pfs::output_iterator_tag
        , output_iterator
        , B, B *, int &>
{
    B * _p;

    output_iterator (output_iterator const & rhs)
        : _p(rhs._p)
    {}

    output_iterator (B * p)
        : _p(p)
    {}

    reference ref ()
    {
        reference r = _p->x;
        _p++;
        return r;
    }
};

TEST_CASE("Output Iterator") {

    B data[4];

    {
        output_iterator it(data);
        *it++ = 1;
        *it++ = 2;
        *it++ = 3;
        *it++ = 4;
        CHECK(data[0].x == 1);
        CHECK(data[1].x == 2);
        CHECK(data[2].x == 3);
        CHECK(data[3].x == 4);
    }
}
