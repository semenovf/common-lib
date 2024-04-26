////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020-2023 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2023.04.12 Initial version.
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/fmt.hpp"
#include "pfs/levenshtein_distance.hpp"
#include "pfs/unicode/utf8_iterator.hpp"
#include <cctype>

struct test_suite_entry
{
    std::size_t wagner_fischer_result;
    std::size_t fast_result;
    char const * a;
    char const * b;
};

struct short_test_suite_entry
{
    std::size_t result;
    char const * a;
    char const * b;
};

struct ignorecase_equality_comparator
{
    inline bool operator () (char a, char b) const noexcept
    {
        return std::tolower(a) == std::tolower(b);
    }
};

#if 1
TEST_CASE("levenshtein_distance")
{
    // Some examples used from https://github.com/wooorm/levenshtein.c
    test_suite_entry test_suite[] = {
          { 0,  0, ""             , ""             }
        , {11, 11, "Levenshtein"  , ""             }
        , {11, 11, ""             , "Levenshtein"  }
        , { 0,  0, "Levenshtein"  , "Levenshtein"  }
        , { 3,  3, "kitten"       , "sitting"      }
        , { 3,  3, "Saturday"     , "Sunday"       }
        , { 2,  2, "gumbo"        , "gambol"       }
        , { 2,  2, "DwAyNE"       , "DUANE"        }
        , { 5,  5, "dwayne"       , "DuAnE"        }
        , { 1,  1, "aarrgh"       , "aargh"        }
        , { 1,  1, "aargh"        , "aarrgh"       }
        , { 1,  1, "a"            , "b"            }
        , { 1,  1, "ab"           , "ac"           }
        , { 1,  1, "ac"           , "bc"           }
        , { 1,  1, "abc"          , "axc"          }
        , { 6,  6, "xabxcdxxefxgx", "1ab2cd34ef5g6"}
        , { 6,  6, "xabxcdxxefxgx", "abcdefg"      }
        , { 7,  7, "javawasneat"  , "scalaisgreat" }
        , { 3,  3, "example"      , "samples"      }
        , { 6,  6, "sturgeon"     , "urgently"     }
        , { 6,  6, "levenshtein"  , "frankenstein" }
        , { 5,  5, "distance"     , "difference"   }
    };

    auto wagner_fischer = pfs::levenshtein_distance_algo::wagner_fischer;
    auto fast = pfs::levenshtein_distance_algo::fast;
    auto myers1999 = pfs::levenshtein_distance_algo::myers1999;
    auto hamming = pfs::levenshtein_distance_algo::hamming;

    // Special cases
    CHECK_THROWS(pfs::levenshtein_distance<>(nullptr      , "Levenshtein", wagner_fischer));
    CHECK_THROWS(pfs::levenshtein_distance<>("Levenshtein", nullptr      , wagner_fischer));
    CHECK_THROWS(pfs::levenshtein_distance<>(nullptr      , nullptr      , wagner_fischer));

    CHECK_THROWS(pfs::levenshtein_distance<>(nullptr      , "Levenshtein", wagner_fischer));
    CHECK_THROWS(pfs::levenshtein_distance<>("Levenshtein", nullptr      , wagner_fischer));
    CHECK_THROWS(pfs::levenshtein_distance<>(nullptr      , nullptr      , wagner_fischer));

    CHECK_EQ(pfs::levenshtein_distance<>("", "", hamming), 0);
    CHECK_EQ(pfs::levenshtein_distance<>("a", "", hamming), 1);
    CHECK_EQ(pfs::levenshtein_distance<>("", "b", hamming), 1);
    CHECK_EQ(pfs::levenshtein_distance<>("aa", "ab", hamming), 1);
    CHECK_THROWS(pfs::levenshtein_distance<>("a", "ab", hamming));

    for (auto const & x: test_suite) {
        CHECK_MESSAGE(pfs::levenshtein_distance<>(x.a, x.b, wagner_fischer) == x.wagner_fischer_result
            , fmt::format(R"(levenshtein_distance("{}", "{}", wagner_fischer))", x.a, x.b));
        CHECK_MESSAGE(pfs::levenshtein_distance<>(x.a, x.b, fast) == x.fast_result
            , fmt::format(R"(levenshtein_distance("{}", "{}", fast))", x.a, x.b));
        CHECK_MESSAGE(pfs::levenshtein_distance<>(x.a, x.b, myers1999) == x.fast_result
            , fmt::format(R"(levenshtein_distance("{}", "{}", myers1999))", x.a, x.b));

        if (std::strcmp(x.a, x.b) == 0) {
            CHECK_MESSAGE(pfs::levenshtein_distance<>(x.a, x.b, hamming) == x.fast_result
                , fmt::format(R"(levenshtein_distance("{}", "{}", hamming))", x.a, x.b));
        }
    }
}
#endif

#if 1
TEST_CASE("levenshtein_distance ignore case")
{
    // Some examples used from https://github.com/wooorm/levenshtein.c
    test_suite_entry test_suite[] = {
          { 0,  0, "ABC"       , "abc"   }
        , { 0,  0, "ABC"       , "aBc"   }
        , { 0,  0, "AbC"       , "aBc"   }
        , { 0,  0, "abC"       , "ABc"   }
        , { 2,  2, "dwayne"    , "DuAnE" }
    };

    auto wagner_fischer = pfs::levenshtein_distance_algo::wagner_fischer;
    auto fast = pfs::levenshtein_distance_algo::fast;

    for (auto const & x: test_suite) {
        CHECK_EQ(pfs::levenshtein_distance<char const *
            , ignorecase_equality_comparator>(x.a, x.b, wagner_fischer)
            , x.wagner_fischer_result);
        CHECK_EQ(pfs::levenshtein_distance<char const *
            , ignorecase_equality_comparator>(x.a, x.b, fast)
            , x.fast_result);
    }
}
#endif

TEST_CASE("levenshtein_distance for UTF8")
{
    using utf8_input_iterator = pfs::unicode::utf8_iterator<char const *>;
    using eq_comparator = pfs::default_equality_comparator<pfs::pointer_dereference_t<utf8_input_iterator>>;
    using levenshtein = pfs::levenshtein_distance_env<utf8_input_iterator, eq_comparator>;

    levenshtein distance;

    short_test_suite_entry test_suite[] = {
          { 0, "АБВ"     , "АБВ" }
        , { 1, "АБВ"     , "АБв" }
        , { 1, "А"       , "а"   }
    };

    for (auto const & x: test_suite) {
        auto xbegin = utf8_input_iterator::begin(x.a, x.a + std::strlen(x.a));
        auto ybegin = utf8_input_iterator::begin(x.b, x.b + std::strlen(x.b));

        CHECK_EQ(distance(xbegin, xbegin.end(), ybegin, ybegin.end()), x.result);
    }
}
