////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Vladislav Trifochkin
//
// This file is part of [common-lib](https://github.com/semenovf/pfs-modulus) library.
//
// Changelog:
//      2021.11.23 Initial version.
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define PFS_COMMON__TEST_ENABLED
#include "doctest.h"
#include "pfs/fmt.hpp"
#include "pfs/home_directory_path.hpp"
#include "pfs/windows.hpp"
#include <string>

TEST_CASE("home_directory_path") {
    namespace fs = pfs::filesystem;

    auto home_dir = fs::home_directory_path();

    CHECK(home_dir != fs::path{});
    CHECK(fs::exists(home_dir));

    fmt::print("Home directory: [{}]\n"
#if PFS_COMPILER_MSVC    
        , pfs::windows::utf8_encode(home_dir.c_str()));
#else
        , home_dir.c_str());
#endif
}

