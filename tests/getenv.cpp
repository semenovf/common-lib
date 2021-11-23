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
#include "pfs/bits/compiler.h"
#include "pfs/fmt.hpp"
#include "pfs/getenv.hpp"
#include <string>

TEST_CASE("getenv") {

std::string not_set {"<not-set>"};

#if PFS_COMPILER_MSVC
    fmt::print("USERPROFILE: [{}]\n"
        , pfs::getenv("USERPROFILE").has_value()
            ? *pfs::getenv("USERPROFILE") : not_set);

    fmt::print("HOMEDRIVE  : [{}]\n"
        , pfs::getenv("HOMEDRIVE").has_value()
            ? *pfs::getenv("HOMEDRIVE") : not_set);

    fmt::print("HOMEPATH   : [{}]\n"
        , pfs::getenv("HOMEPATH").has_value()
            ? *pfs::getenv("HOMEPATH") : not_set);

    fmt::print("HOME       : [{}]\n"
        , pfs::getenv("HOME").has_value()
            ? *pfs::getenv("HOME") : not_set);

#elif PFS_COMPILER_GCC

    fmt::print("HOME: [{}]\n"
        , pfs::getenv("HOME").has_value()
            ? *pfs::getenv("HOME") : not_set);

    fmt::print("USER: [{}]\n"
        , pfs::getenv("USER").has_value()
            ? *pfs::getenv("USER") : not_set);

#endif

    fmt::print("NOT-SET: [{}]\n"
        , pfs::getenv("NOT-SET").has_value()
            ? *pfs::getenv("NOT-SET") : not_set);

    CHECK_FALSE(pfs::getenv("NOT-SET").has_value());
}
