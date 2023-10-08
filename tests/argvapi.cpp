////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2023 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2023.10.08 Initial version.
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/argvapi.hpp"
#include <map>
#include <vector>

TEST_CASE("default") {
    using pfs::to_string;

    bool program_name_skipped = true;
    char const * argv[] = {
        "-y", "--yes", "--hello=world", "-another=world", "--", "arg0" , "-"
    };

    pfs::argvapi cl { sizeof(argv) / sizeof(argv[0]), argv, program_name_skipped};
    auto it = cl.begin();

    int double_dash_count = 0;
    int single_dash_count = 0;
    std::map<pfs::string_view, pfs::string_view> opts;
    std::vector<pfs::string_view> args;

    while (it.has_more()) {
        auto x = it.next();

        if (x.is_double_dash())
            double_dash_count++;
        else if (x.is_single_dash())
            single_dash_count++;
        else if (x.is_option())
            opts.emplace(x.optname(), x.arg());
        else if (x.has_arg())
            args.emplace_back(x.arg());
        else {
            fmt::println("Optname: [{}], Arg: [{}]"
                , to_string(x.optname())
                , to_string(x.arg()));
            FAIL("Unexpected");
        }
    }

    CHECK_EQ(double_dash_count, 1);
    CHECK_EQ(single_dash_count, 1);
    CHECK_EQ(opts.at(pfs::string_view{"y"}), pfs::string_view{});
    CHECK_EQ(opts.at(pfs::string_view{"yes"}), pfs::string_view{});
    CHECK_EQ(opts.at(pfs::string_view{"hello"}), pfs::string_view{"world"});
    CHECK_EQ(opts.at(pfs::string_view{"another"}), pfs::string_view{"world"});

    CHECK_EQ(args.at(0), pfs::string_view{"arg0"});
}
