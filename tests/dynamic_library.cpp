////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 Vladislav Trifochkin
//
// This file is part of [pfs-modulus](https://github.com/semenovf/pfs-modulus) library.
//
// Changelog:
//      2019.12.20 Initial version (inhereted from https://github.com/semenovf/pfs)
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/dynamic_library.hpp"

TEST_CASE("Dynamic Library basics") {
    pfs::dynamic_library dl;
    typedef int (* dl_test_fn)(void);

    auto dlfile = "./" + pfs::dynamic_library::build_dl_filename("shared_object");

    std::error_code ec;

    bool rc = dl.open(dlfile, ec);

    REQUIRE_MESSAGE(rc, "Dynamic library open failed: " << ec.message());

    dl_test_fn dltest = pfs::void_func_ptr_cast<dl_test_fn>(
            dl.resolve("dl_only_for_testing_purpose"
                    , ec));

    REQUIRE_MESSAGE(!ec, "Dynamic library symbol resolve failed: " << ec.message());
    MESSAGE("'dl_only_for_testing_purpose': symbol (function pointer) found");
    REQUIRE_MESSAGE(dltest() == 1234, "run plugin/library function");
    MESSAGE("run plugin/library function: success");
}
