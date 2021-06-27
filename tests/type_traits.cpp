////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Vladislav Trifochkin
//
// This file is part of [common-lib](https://github.com/semenovf/common-lib) library.
//
// Changelog:
//      2021.04.26 Initial version
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/type_traits.hpp"

namespace t0 {
    void f () {}

    void (*fptr) ();
    void (*fptr_int) (int);
}

TEST_CASE("is_function_pointer") {
    using std::is_same;
    using std::is_function;
    using pfs::is_function_pointer;

    CHECK(is_function<decltype(t0::f)>::value);
    CHECK_FALSE(pfs::is_function_pointer<decltype(t0::f)>::value);

    CHECK(std::is_pointer<decltype(t0::fptr)>::value);
    CHECK(pfs::is_function_pointer<decltype(t0::fptr)>::value);
    CHECK_FALSE(std::is_function<decltype(t0::fptr)>::value);

    CHECK_FALSE(pfs::is_function_pointer<int>::value);

    CHECK(is_same<decltype(t0::fptr), void (*) ()>::value);
    CHECK(is_same<decltype(t0::fptr_int), void (*) (int)>::value);
}
