////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 Vladislav Trifochkin
//
// This file is part of [pfs-common](https://github.com/semenovf/pfs-common) library.
//
// Changelog:
//      2020.03.14 Initial version (inspired from https://github.com/semenovf/pfs)
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/compare.hpp"

class A : public pfs::compare_op<A>, public pfs::compare_op<A, int>
{
public:
    int x {0};

public:
    A () {}
    explicit A (int x) : x(x) {}

    int compare (A const & rhs) const
    {
        return x - rhs.x;
    }

    int compare (int rhs) const
    {
        return x - rhs;
    }
};

class B : public pfs::compare_op<B>, public pfs::compare_op<B, A>
{
public:
    int x {0};

public:
    B () {}
    explicit B (int x) : x(x) {}

    int compare (B const & rhs) const
    {
        return x - rhs.x;
    }

    int compare (A const & rhs) const
    {
        return x - rhs.x;
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
    A a2 {0};
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
