////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2019.12.20 Initial version (inhereted from https://github.com/semenovf/pfs)
//      2021.12.29 Refactored according to RAII idiom.
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/dynamic_library.hpp"
#include <type_traits>

#if PFS_HAVE_STD_FILESYSTEM
namespace fs = std::filesystem;
#else
namespace fs = pfs::filesystem;
#endif

TEST_CASE("constructors") {
    REQUIRE_FALSE(std::is_default_constructible<pfs::dynamic_library>::value);
    REQUIRE_FALSE(std::is_copy_constructible<pfs::dynamic_library>::value);
    REQUIRE_FALSE(std::is_copy_assignable<pfs::dynamic_library>::value);
    REQUIRE(std::is_move_constructible<pfs::dynamic_library>::value);
    REQUIRE(std::is_move_assignable<pfs::dynamic_library>::value);
    REQUIRE(std::is_destructible<pfs::dynamic_library>::value);
}

TEST_CASE("Dynamic Library basics") {
#if (defined(_WIN32) || defined(_WIN64)) && defined(_UNICODE)
    auto dlfile = L"./" + pfs::dynamic_library::build_filename(L"shared_object");
#else
    auto dlfile = "./" + pfs::dynamic_library::build_filename("shared_object");
#endif

    pfs::error err;
    pfs::dynamic_library dl {dlfile, & err};

    REQUIRE_MESSAGE((dl), "Dynamic library open failed: " << err.what());

    auto dltest = dl.resolve<int(void)>("dl_only_for_testing_purpose", & err);

    REQUIRE_MESSAGE(!err, "Dynamic library symbol resolve failed: " << err.what());
    MESSAGE("'dl_only_for_testing_purpose': symbol (function pointer) found");
    REQUIRE_MESSAGE(dltest() == 1234, "run plugin/library function");
    MESSAGE("run plugin/library function: success");
}
