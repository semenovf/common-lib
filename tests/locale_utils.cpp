////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2022 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2022.06.10 Initial version.
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/fmt.hpp"
#include "pfs/locale_utils.hpp"
#include <locale>
#include <string>

TEST_CASE("locale_utils") {
    std::locale local_context {""};

    fmt::print("Local context:\n");

    // Ouput on Linux / POSIX (as single line):
    // LC_CTYPE=en_US.UTF-8;LC_NUMERIC=ru_RU.UTF-8;LC_TIME=ru_RU.UTF-8;
    // LC_COLLATE=en_US.UTF-8;LC_MONETARY=ru_RU.UTF-8;LC_MESSAGES=en_US.UTF-8;
    // LC_PAPER=ru_RU.UTF-8;LC_NAME=ru_RU.UTF-8;LC_ADDRESS=ru_RU.UTF-8;
    // LC_TELEPHONE=ru_RU.UTF-8;LC_MEASUREMENT=ru_RU.UTF-8;
    // LC_IDENTIFICATION=ru_RU.UTF-8
    fmt::print("{}\n", local_context.name());

    std::string locname = "LC_CTYPE=en_US.UTF-8;LC_NUMERIC=ru_RU.UTF-8"
        ";LC_TIME=ru_RU.UTF-8;LC_COLLATE=en_US.UTF-8;LC_MONETARY=ru_RU.UTF-8"
        ";LC_MESSAGES=en_US.UTF-8;LC_PAPER=ru_RU.UTF-8;LC_NAME=ru_RU.UTF-8"
        ";LC_ADDRESS=ru_RU.UTF-8; \tLC_TELEPHONE=ru_RU.UTF-8"
        ";LC_MEASUREMENT=ru_RU.UTF-8;\tLC_IDENTIFICATION=ru_RU.UTF-8";

    auto categories = pfs::parse_locale_name(locname);

    std::map<std::string, std::string> sample {
          {"LC_CTYPE"         , "en_US.UTF-8"}
        , {"LC_NUMERIC"       , "ru_RU.UTF-8"}
        , {"LC_TIME"          , "ru_RU.UTF-8"}
        , {"LC_COLLATE"       , "en_US.UTF-8"}
        , {"LC_MONETARY"      , "ru_RU.UTF-8"}
        , {"LC_MESSAGES"      , "en_US.UTF-8"}
        , {"LC_PAPER"         , "ru_RU.UTF-8"}
        , {"LC_NAME"          , "ru_RU.UTF-8"}
        , {"LC_ADDRESS"       , "ru_RU.UTF-8"}
        , {"LC_TELEPHONE"     , "ru_RU.UTF-8"}
        , {"LC_MEASUREMENT"   , "ru_RU.UTF-8"}
        , {"LC_IDENTIFICATION", "ru_RU.UTF-8"}
    };

    CHECK_EQ(categories.size(), sample.size());

    for (auto const & cat: categories) {
        CHECK_EQ(cat.second, sample[cat.first]);
    }

    {
        auto ctx = pfs::parse_locale_category("en_US.UTF-8");
        CHECK_EQ(ctx.lang, "en");
        CHECK_EQ(ctx.zone, "US");
        CHECK_EQ(ctx.encoding, "UTF-8");
        CHECK_EQ(ctx.mod, "");
    }

    {
        auto ctx = pfs::parse_locale_category("de_DE.UTF-8@euro");
        CHECK_EQ(ctx.lang, "de");
        CHECK_EQ(ctx.zone, "DE");
        CHECK_EQ(ctx.encoding, "UTF-8");
        CHECK_EQ(ctx.mod, "euro");
    }

    {
        auto ctx = pfs::parse_locale_category("deu_deu.1252");
        CHECK_EQ(ctx.lang, "deu");
        CHECK_EQ(ctx.zone, "deu");
        CHECK_EQ(ctx.encoding, "1252");
        CHECK_EQ(ctx.mod, "");
    }

    {
        auto ctx = pfs::parse_locale_category("POSIX");
        CHECK_EQ(ctx.lang, "POSIX");
        CHECK_EQ(ctx.zone, "");
        CHECK_EQ(ctx.encoding, "");
        CHECK_EQ(ctx.mod, "");
    }
}
