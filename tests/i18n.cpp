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
#include "pfs/i18n.hpp"
#include <string>

/*
 * static char const * STATIC_CHARS = tr::noop_("STATIC CHARS");
 * static std::string STATIC_STRING = tr::noop_("STATIC STRING");
 *
 * int main ()
 * {
 *      auto domaindir = std::string{getenv("PWD")} + "/locale";
 *
 *      NOTE! LC_ALL gives success result instead of LC_MESSAGES
 *      //auto loc = std::setlocale(LC_MESSAGES, "");
 *
 *      // Default (system) locale
 *      auto loc = std::setlocale(LC_ALL, "");
 *
 *      // Explicitly set locale
 *      //auto loc = std::setlocale(LC_ALL, "ru_RU.UTF-8");
 *      //auto loc = std::setlocale(LC_ALL, "ru_RU.utf8");
 *
 *      //                  ------------------ Basename for mo-file
 *      //                  |          ------- Better use absolute path for dirname
 *      //                  |          |
 *      //                  v          v
 *      bindtextdomain("translation", domaindir.c_str());
 *      auto tdom = textdomain("translation");
 *
 *      std::printf("Locale: %s\n", loc);
 *      std::printf("Domain dir: %s\n", bindtextdomain("translation", nullptr));
 *      std::printf("Text domain: %s\n", tdom);
 *
 *     auto text1 = tr::f_("Hello, {}", tr::_(STATIC_CHARS));
 *     auto text2 = tr::f_("Hello, {}", tr::_(STATIC_STRING));
 *     auto text3 = tr::f_("Hello, {}", tr::_("World"));
 *     auto text4 = tr::fn_({"{} day left", "{} days left", 0}, 0);
 * }
 *
 * -----------------------------------------------------------------------------
 *
 * $ xgettext --keyword=_ --keyword=f_ --keyword=n_:1,2 --keyword=noop_ --output=/tmp/tr.pot tr.cpp
 */

static char const * STATIC_CHARS = tr::noop_("STATIC CHARS");
static std::string STATIC_STRING = tr::noop_("STATIC STRING");
static char const * DOMAIN_STATIC_CHARS = tr::noop_("DOMAIN STATIC CHARS");
static std::string DOMAIN_STATIC_STRING = tr::noop_("DOMAIN STATIC STRING");
static char const * FORMAT_STATIC_CHARS = tr::noop_("FORMAT STATIC CHARS");
static std::string FORMAT_STATIC_STRING = tr::noop_("FORMAT STATIC STRING");

TEST_CASE("translate") {
    {
        auto text1 = tr::_(STATIC_CHARS);
        auto text2 = tr::_(STATIC_STRING);
        auto text3 = tr::_("Raw C string");

        std::string s4{"C++ string"};
        auto text4 = tr::_(s4);

        CHECK_EQ(text1, std::string{"STATIC CHARS"});
        CHECK_EQ(text2, std::string{"STATIC STRING"});
        CHECK_EQ(text3, std::string{"Raw C string"});
        CHECK_EQ(text4, std::string{"C++ string"});
    }

    // Domain name and category
    {
        auto text1 = tr::_(DOMAIN_STATIC_CHARS, {"domain", LC_MESSAGES});
        auto text2 = tr::_(DOMAIN_STATIC_STRING, {"domain", LC_MESSAGES});
        auto text3 = tr::_("Domain Raw C string", {"domain", LC_MESSAGES});

        std::string s4 {"Domain C++ string"};
        auto text4 = tr::_(s4, {"domain", LC_MESSAGES});

        CHECK_EQ(text1, std::string{"DOMAIN STATIC CHARS"});
        CHECK_EQ(text2, std::string{"DOMAIN STATIC STRING"});
        CHECK_EQ(text3, std::string{"Domain Raw C string"});
        CHECK_EQ(text4, std::string{"Domain C++ string"});
    }

    // Plural
    {
        auto text1 = fmt::format(tr::n_({"{} day left", "{} days left", 0}), 0);
        auto text2 = fmt::format(tr::n_({"{} day left", "{} days left", 1}), 1);
        auto text3 = fmt::format(tr::n_({"{} day left", "{} days left", 2}), 2);
        auto text4 = fmt::format(tr::n_({"{} day left", "{} days left", 5}), 5);
        auto text5 = fmt::format(tr::n_({"{} day left", "{} days left", 42}), 42);

        CHECK_EQ(text1, std::string{"0 days left"});
        CHECK_EQ(text2, std::string{"1 day left"});
        CHECK_EQ(text3, std::string{"2 days left"});
        CHECK_EQ(text4, std::string{"5 days left"});
        CHECK_EQ(text5, std::string{"42 days left"});
    }

    // Plural, domain name and category
    {
        auto text1 = fmt::format(tr::n_({"Domain: {} day left", "Domain: {} days left", 0}, {"domain", LC_MESSAGES}), 0);
        auto text2 = fmt::format(tr::n_({"Domain: {} day left", "Domain: {} days left", 1}, {"domain", LC_MESSAGES}), 1);
        auto text3 = fmt::format(tr::n_({"Domain: {} day left", "Domain: {} days left", 2}, {"domain", LC_MESSAGES}), 2);
        auto text4 = fmt::format(tr::n_({"Domain: {} day left", "Domain: {} days left", 5}, {"domain", LC_MESSAGES}), 5);
        auto text5 = fmt::format(tr::n_({"Domain: {} day left", "Domain: {} days left", 42}, {"domain", LC_MESSAGES}), 42);

        CHECK_EQ(text1, std::string{"Domain: 0 days left"});
        CHECK_EQ(text2, std::string{"Domain: 1 day left"});
        CHECK_EQ(text3, std::string{"Domain: 2 days left"});
        CHECK_EQ(text4, std::string{"Domain: 5 days left"});
        CHECK_EQ(text5, std::string{"Domain: 42 days left"});
    }

    // Format
    {
        auto text1 = tr::f_("Hello, {}", tr::_(FORMAT_STATIC_CHARS));
        auto text2 = tr::f_("Hello, {}", tr::_(FORMAT_STATIC_STRING));
        auto text3 = tr::f_("Hello, {}", tr::_("World!"));
        auto text4 = tr::f_("Hello, {}", {"domain", LC_MESSAGES}, tr::_("Domain!"));

        CHECK_EQ(text1, std::string{"Hello, FORMAT STATIC CHARS"});
        CHECK_EQ(text2, std::string{"Hello, FORMAT STATIC STRING"});
        CHECK_EQ(text3, std::string{"Hello, World!"});
        CHECK_EQ(text4, std::string{"Hello, Domain!"});
    }
}
