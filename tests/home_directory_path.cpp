////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2021.11.23 Initial version.
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define PFS__TEST_ENABLED
#include "doctest.h"
#include "pfs/fmt.hpp"
#include "pfs/home_directory_path.hpp"
#include "pfs/windows.hpp"
#include <string>

TEST_CASE("home_directory_path") {
    namespace fs = pfs::filesystem;

    auto home_dir = fs::home_directory_path();

    if (!home_dir.empty()) {
        CHECK(fs::exists(home_dir));
        fmt::print("Home directory: [{}]\n"    
#if PFS__COMPILER_MSVC
            , pfs::windows::utf8_encode(home_dir.c_str()));
#else
            , home_dir.c_str());
#endif
    } else {
        MESSAGE("Home directory is not defined for this operation system");
    }

}

