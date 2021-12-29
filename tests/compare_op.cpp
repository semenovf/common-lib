////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2020.03.14 Initial version (inspired from https://github.com/semenovf/pfs)
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/compare.hpp"

class A : public pfs::compare_operations
{
public:
    int x {0};

public:
    A () {}
    explicit A (int x) : x(x) {}

    friend bool operator == (A const & a, A const & b)
    {
        return a.x == b.x;
    }

    friend bool operator < (A const & a, A const & b)
    {
        return a.x < b.x;
    }

    friend bool operator == (A const & a, int b)
    {
        return a.x == b;
    }

    friend bool operator == (int a, A const & b)
    {
        return a == b.x;
    }

    friend bool operator < (A const & a, int b)
    {
        return a.x < b;
    }

    friend bool operator < (int a, A const & b)
    {
        return a < b.x;
    }
};

class B : public pfs::compare_operations
{
public:
    int x {0};

public:
    B () {}
    explicit B (int x) : x(x) {}

    friend bool operator == (B const & a, B const & b)
    {
        return a.x == b.x;
    }

    friend bool operator < (B const & a, B const & b)
    {
        return a.x < b.x;
    }

    friend bool operator == (B const & a, A const & b)
    {
        return a.x == b.x;
    }

    friend bool operator < (B const & a, A const & b)
    {
        return a.x < b.x;
    }

    friend bool operator == (A const & a, B const & b)
    {
        return a == b.x;
    }

    friend bool operator < (A const & a, B const & b)
    {
        return a < b.x;
    }
};

TEST_CASE("Compare same types") {
    A a1{-1}, a2{0}, a3{1};

    CHECK(a1 == a1);
    CHECK_FALSE(a1 == a2);
    CHECK_FALSE(a1 == a3);

    CHECK(a1 != a2);
    CHECK(a1 != a3);
    CHECK_FALSE(a1 != a1);

    CHECK(a1 < a2);
    CHECK(a1 < a3);
    CHECK_FALSE(a2 < a1);
    CHECK_FALSE(a3 < a1);

    CHECK(a1 <= a2);
    CHECK(a1 <= a3);
    CHECK_FALSE(a2 <= a1);
    CHECK_FALSE(a3 <= a1);

    CHECK(a2 > a1);
    CHECK(a3 > a1);
    CHECK_FALSE(a1 > a2);
    CHECK_FALSE(a1 > a3);

    CHECK(a2 >= a1);
    CHECK(a3 >= a1);
    CHECK_FALSE(a1 >= a2);
    CHECK_FALSE(a1 >= a3);
}

TEST_CASE("Compare with scalars") {
    A a1 {-1};
    int a2 {0};
    int a3 {1};

    CHECK(a1 == a1);
    CHECK_FALSE(a1 == a2);
    CHECK_FALSE(a1 == a3);
    CHECK_FALSE(a2 == a1);
    CHECK_FALSE(a3 == a1);

    CHECK(a1 != a2);
    CHECK(a1 != a3);
    CHECK(a2 != a1);
    CHECK(a3 != a1);
    CHECK_FALSE(a1 != a1);

    CHECK(a1 < a2);
    CHECK(a1 < a3);
    CHECK_FALSE(a2 < a1);
    CHECK_FALSE(a3 < a1);

    CHECK(a1 <= a2);
    CHECK(a1 <= a3);
    CHECK_FALSE(a2 <= a1);
    CHECK_FALSE(a3 <= a1);

    CHECK(a2 > a1);
    CHECK(a3 > a1);
    CHECK_FALSE(a1 > a2);
    CHECK_FALSE(a1 > a3);

    CHECK(a2 >= a1);
    CHECK(a3 >= a1);
    CHECK_FALSE(a1 >= a2);
    CHECK_FALSE(a1 >= a3);
}

TEST_CASE("Compare difference types") {
    B a1 {-1};
    B a2 {0};
    A a3 {1};

    CHECK(a1 == a1);
    CHECK_FALSE(a1 == a2);
    CHECK_FALSE(a1 == a3);
    CHECK_FALSE(a2 == a1);
    CHECK_FALSE(a3 == a1);

    CHECK(a1 != a2);
    CHECK(a1 != a3);
    CHECK(a2 != a1);
    CHECK(a3 != a1);
    CHECK_FALSE(a1 != a1);

    CHECK(a1 < a2);
    CHECK(a1 < a3);
    CHECK_FALSE(a2 < a1);
    CHECK_FALSE(a3 < a1);

    CHECK(a1 <= a2);
    CHECK(a1 <= a3);
    CHECK_FALSE(a2 <= a1);
    CHECK_FALSE(a3 <= a1);

    CHECK(a2 > a1);
    CHECK(a3 > a1);
    CHECK_FALSE(a1 > a2);
    CHECK_FALSE(a1 > a3);

    CHECK(a2 >= a1);
    CHECK(a3 >= a1);
    CHECK_FALSE(a1 >= a2);
    CHECK_FALSE(a1 >= a3);
}
