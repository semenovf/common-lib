////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of [pfs-modulus](https://github.com/semenovf/pfs-modulus) library.
//
// Changelog:
//      2019.??.?? Initial version
//      2021.04.25 Moved from pfs-modulus into pfs-common
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/source_location.hpp"
// #include <string>

TEST_CASE("source_location") {
    using source_location = pfs::source_location;

    source_location loc = PFS_CURRENT_SOURCE_LOCATION; auto __test_line = __LINE__;

    std::cout << loc.file_name()
        << ':' << loc.line()
        << ": " << loc.function_name() << "\n";

    CHECK_EQ(loc.line(), __test_line);
}
