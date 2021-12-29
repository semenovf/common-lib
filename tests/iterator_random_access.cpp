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

struct E
{
    int x;
};

class random_access_iterator : public pfs::iterator_facade<
          pfs::random_access_iterator_tag
        , random_access_iterator
        , E, E *, E &>
{
private:
    E * _p;

public:
    random_access_iterator (E * p)
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

    void increment (difference_type n)
    {
        _p += n;
    }

    int compare (random_access_iterator const & rhs) const
    {
        return _p - rhs._p;
    }

    void decrement (difference_type n)
    {
        _p -= n;
    }

    reference subscript (difference_type n)
    {
        return *(_p + n);
    }

    difference_type diff (random_access_iterator const & rhs) const
    {
        return _p - rhs._p;
    }
};

TEST_CASE("Random Access Iterator") {

    E data[] = { {1}, {2}, {3}, {4} };

    // Test postfix increment
    {
        random_access_iterator it(data);

        CHECK((*it++).x == 1);
        CHECK((*it++).x == 2);
        CHECK((*it++).x == 3);
        CHECK((*it++).x == 4);
    }

    // Test prefix increment
    {
        random_access_iterator it(data);
        CHECK((*it).x == 1);
        CHECK((*++it).x == 2);
        CHECK((*++it).x == 3);
        CHECK((*++it).x == 4);
    }

    // Test postfix decrement
    {
        random_access_iterator it(data + sizeof(data)/sizeof(data[0]));

        it--;
        CHECK((*it--).x == 4);
        CHECK((*it--).x == 3);
        CHECK((*it--).x == 2);
        CHECK((*it--).x == 1);
    }

    // Test prefix decrement
    {
        random_access_iterator it(data + sizeof(data)/sizeof(data[0]));

        CHECK((*--it).x == 4);
        CHECK((*--it).x == 3);
        CHECK((*--it).x == 2);
        CHECK((*--it).x == 1);
    }

    // Test equality and `->` operator
    {
        random_access_iterator it1(data);
        random_access_iterator it2(data);

        CHECK(it1 == it2++);
        CHECK(it1 != it2);

        CHECK(it1->x == 1);
        CHECK(it2->x == 2);
    }

    // Test subscript
    {
        random_access_iterator it(data);
        CHECK(it[0].x == 1);
        CHECK(it[1].x == 2);
        CHECK(it[2].x == 3);
        CHECK(it[3].x == 4);
    }

    // Test +=, -=
    {
        random_access_iterator it(data);

        it += 0;
        CHECK(it->x == 1);

        it -= 0;
        CHECK(it->x == 1);

        it += 1;
        CHECK(it->x == 2);

        it -= 1;
        CHECK(it->x == 1);

        it += 2;
        CHECK(it->x == 3);

        it -= 2;
        CHECK(it->x == 1);

        it += 3;
        CHECK(it->x == 4);

        it -= 3;
        CHECK(it->x == 1);

        random_access_iterator it1(data + sizeof(data)/sizeof(data[0]));

        it1 -= sizeof(data)/sizeof(data[0]);
        CHECK(it1->x == 1);

        CHECK((it1 + 0)->x == 1);
        CHECK((it1 + 1)->x == 2);
        CHECK((it1 + 2)->x == 3);
        CHECK((it1 + 3)->x == 4);

        it1 += sizeof(data)/sizeof(data[0]);
        CHECK((it1 - 1)->x == 4);
        CHECK((it1 - 2)->x == 3);
        CHECK((it1 - 3)->x == 2);
        CHECK((it1 - 4)->x == 1);
    }

    // Test difference
    {
        random_access_iterator it1(data);
        random_access_iterator it2(data + sizeof(data)/sizeof(data[0]));

        CHECK(it2 - it1 == sizeof(data)/sizeof(data[0]));
        CHECK(it1 - it2 == - static_cast<int>(sizeof(data)/sizeof(data[0])));
    }

    // Test compare
    {
        random_access_iterator it1(data);
        random_access_iterator it2(data);
        random_access_iterator it3(data + sizeof(data)/sizeof(data[0]));

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
