////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of [pfs-common](https://github.com/semenovf/pfs-common) library.
//
// Changelog:
//      2020.03.28 Initial version
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/rational.hpp"
#include <algorithm>
#include <string>

using rational = pfs::rational<int>;

TEST_CASE("Constructors") {
    CHECK(to_string(rational())         == std::string("0/1"));
    CHECK(to_string(rational(1))        == std::string("1/1"));
    CHECK(to_string(rational(-1))       == std::string("-1/1"));
    CHECK(to_string(rational(1, 1))     == std::string("1/1"));
    CHECK(to_string(rational(-1, 1))    == std::string("-1/1"));
    CHECK(to_string(rational(1, -1))    == std::string("-1/1"));
    CHECK(to_string(rational(-1, -1))   == std::string("1/1"));
    CHECK(to_string(rational(2, 1))     == std::string("2/1"));
    CHECK(to_string(rational(7, 1))     == std::string("7/1"));
    CHECK(to_string(rational(4, 2))     == std::string("4/2"));
    CHECK(to_string(rational(7, 10))    == std::string("7/10"));
    CHECK(to_string(rational(7, 100))   == std::string("7/100"));
    CHECK(to_string(rational(7, 1000))  == std::string("7/1000"));
    CHECK(to_string(rational(7, 10000)) == std::string("7/10000"));
    CHECK(to_string(rational(7, 11))    == std::string("7/11"));

    CHECK(to_string(rational(std::ratio<1, 1>()))     == std::string("1/1"));
    CHECK(to_string(rational(std::ratio<-1, 1>()))    == std::string("-1/1"));
    CHECK(to_string(rational(std::ratio<1, -1>()))    == std::string("-1/1"));
    CHECK(to_string(rational(std::ratio<-1, -1>()))   == std::string("1/1"));
    CHECK(to_string(rational(std::ratio<2, 1>()))     == std::string("2/1"));
    CHECK(to_string(rational(std::ratio<7, 1>()))     == std::string("7/1"));
    CHECK(to_string(rational(std::ratio<4, 2>()))     == std::string("2/1"));
    CHECK(to_string(rational(std::ratio<7, 10>()))    == std::string("7/10"));
    CHECK(to_string(rational(std::ratio<7, 100>()))   == std::string("7/100"));
    CHECK(to_string(rational(std::ratio<7, 1000>()))  == std::string("7/1000"));
    CHECK(to_string(rational(std::ratio<7, 10000>())) == std::string("7/10000"));
    CHECK(to_string(rational(std::ratio<7, 11>()))    == std::string("7/11"));
}

TEST_CASE("Assignment") {
    rational a;
    CHECK((a = rational(7, 11)) == rational(7, 11));
    CHECK((a.assign(8, 12)) == rational(8, 12));
}

TEST_CASE("Unary substract") {
    CHECK(to_string(-rational(1))      == std::string("-1/1"));
    CHECK(to_string(-rational(-1))     == std::string("1/1"));
    CHECK(to_string(-rational(1, 1))   == std::string("-1/1"));
    CHECK(to_string(-rational(-1, 1))  == std::string("1/1"));
    CHECK(to_string(-rational(1, -1))  == std::string("1/1"));
    CHECK(to_string(-rational(-1, -1)) == std::string("-1/1"));
    CHECK(to_string(-rational(7, 11))  == std::string("-7/11"));
    CHECK(to_string(-rational(7, -11)) == std::string("7/11"));
    CHECK(to_string(-rational(-7, 11)) == std::string("7/11"));
}

TEST_CASE("Unary add") {
    CHECK(to_string(+rational(1))      == std::string("1/1"));
    CHECK(to_string(+rational(-1))     == std::string("-1/1"));
    CHECK(to_string(+rational(1, 1))   == std::string("1/1"));
    CHECK(to_string(+rational(-1, 1))  == std::string("-1/1"));
    CHECK(to_string(+rational(1, -1))  == std::string("-1/1"));
    CHECK(to_string(+rational(-1, -1)) == std::string("1/1"));
    CHECK(to_string(+rational(7, 11))  == std::string("7/11"));
    CHECK(to_string(+rational(7, -11)) == std::string("-7/11"));
    CHECK(to_string(+rational(-7, 11)) == std::string("-7/11"));
}

TEST_CASE("Absolute value") {
    CHECK(rational(1).abs()       == rational(1));
    CHECK(rational(-1).abs()      == rational(1));
    CHECK(rational( 1, 1).abs()   == rational(1));
    CHECK(rational(-1, 1).abs()   == rational(1));
    CHECK(rational( 1, -1).abs()  == rational(1));

    CHECK(rational( 7, 11).abs()  == rational(7, 11));
    CHECK(rational( 7, -11).abs() == rational(7, 11));
    CHECK(rational(-7, 11).abs()  == rational(7, 11));
    CHECK(rational(-7,-11).abs()  == rational(7, 11));
}

TEST_CASE("Reduce") {
    CHECK(to_string(rational( 1, 2).reduce())  == "1/2");
    CHECK(to_string(rational( 2, 4).reduce())  == "1/2");
    CHECK(to_string(rational(-8, 16).reduce()) == "-1/2");
}

TEST_CASE("Invert") {
    CHECK(to_string(rational( 1, 2).invert())  == std::string("2/1"));
    CHECK(to_string(rational( 4, 2).invert())  == std::string("2/4"));
    CHECK(to_string(rational(-8, 16).invert()) == std::string("-16/8"));
}

TEST_CASE("Is decimal") {
    CHECK(!rational( 1, 2).is_decimal());
    CHECK(!rational( 1, 110).is_decimal());
    CHECK(rational( 3, 10).is_decimal());
    CHECK(rational( 3, 100000).is_decimal());
}

TEST_CASE("Compare") {
    // Denominators are equals
    CHECK(rational(-7, 11).compare(rational(-7, 11)) == 0);
    CHECK(rational(-7, 11).compare(rational( 7, 11)) <  0);
    CHECK(rational( 7, 11).compare(rational(-7, 11)) >  0);

    // Different signs
    CHECK(rational(-7, 11).compare(rational( 7, 12)) <  0);
    CHECK(rational( 7, 11).compare(rational(-7, 12)) >  0);

    // Different Quotients (q = num / den)
    CHECK(rational( 12, 11).compare(rational( 26, 12)) <  0);
    CHECK(rational( 26, 12).compare(rational( 12, 11)) >  0);
    CHECK(rational(-12, 11).compare(rational(-26, 12)) >  0);
    CHECK(rational(-26, 12).compare(rational(-12, 11)) <  0);

    CHECK(rational( 555, 5).compare(rational( 447, 4)) <  0);
    CHECK(rational( 447, 4).compare(rational( 555, 5)) >  0);
    CHECK(rational(-555, 5).compare(rational(-447, 4)) >  0);
    CHECK(rational(-447, 4).compare(rational(-555, 5)) <  0);
    CHECK(rational( 555, 5).compare(rational( 222, 2)) ==  0);
    CHECK(rational(-555, 5).compare(rational(-222, 2)) ==  0);

    CHECK(rational(2, std::numeric_limits<rational::int_type>::max())
            .compare(rational( 4, 5)) <  0);

    CHECK(rational(std::numeric_limits<rational::int_type>::max() - 2, std::numeric_limits<rational::int_type>::max())
            .compare(rational(std::numeric_limits<rational::int_type>::max() - 1, std::numeric_limits<rational::int_type>::max())) <  0);

    CHECK(rational(-(std::numeric_limits<rational::int_type>::max() - 2), std::numeric_limits<rational::int_type>::max())
            .compare(-(rational(std::numeric_limits<rational::int_type>::max() - 1), std::numeric_limits<rational::int_type>::max())) >  0);

    CHECK(rational(std::numeric_limits<rational::int_type>::min(), std::numeric_limits<rational::int_type>::max())
            .compare(rational(std::numeric_limits<rational::int_type>::min(), std::numeric_limits<rational::int_type>::max())) == 0);

    CHECK(rational(std::numeric_limits<rational::int_type>::min() + 1, std::numeric_limits<rational::int_type>::max())
            .compare(rational(std::numeric_limits<rational::int_type>::min() + 2, std::numeric_limits<rational::int_type>::max())) <  0);

    CHECK(rational(std::numeric_limits<rational::int_type>::min() + 2, std::numeric_limits<rational::int_type>::max())
            .compare(rational(std::numeric_limits<rational::int_type>::min() + 1, std::numeric_limits<rational::int_type>::max())) >  0);

    CHECK(rational(-7, 11) == rational(-7, 11));
    CHECK(rational(-7, 11) != rational( 7, 11));
    CHECK(rational(-7, 11) < rational( 7, 11));
    CHECK(rational(-7, 11) <= rational( 7, 11));
    CHECK(rational( 7, 11) > rational(-7, 11));
    CHECK(rational( 7, 11) >= rational(-7, 11));
}

TEST_CASE("Arithmetic operators") {
    CHECK((rational(-7, 11) += rational(0)) == rational(-7, 11));
    CHECK((rational(-7, 11) += rational( 7, 11)) == rational(0));
    CHECK((rational( 7, 11) += rational( 7, 11)) == rational( 14, 11));
    CHECK((rational(-7, 11) += rational(-7, 11)) == rational(-14, 11));

    CHECK((rational(-7, 11) -= rational(0)) == rational(-7, 11));
    CHECK((rational(-7, 11) -= rational(-7, 11)) == rational(0));
    CHECK((rational( 7, 11) -= rational(-7, 11)) == rational( 14, 11));
    CHECK((rational(-7, 11) -= rational( 7, 11)) == rational(-14, 11));

    CHECK((rational(-7, 11) *= rational(0)) == rational(0));
    CHECK((rational(-7, 11) *= rational(-7, 11)) == rational( 49, 121));
    CHECK((rational( 7, 11) *= rational(-7, 11)) == rational(-49, 121));
    CHECK((rational(-7, 11) *= rational( 7, 11)) == rational(-49, 121));

    CHECK((rational(0) /= rational(-7, 11)) == rational(0));
    CHECK((rational( 49, 121) /= rational(-7, 11)) == rational(-7, 11));
    CHECK((rational(-49, 121) /= rational(-7, 11)) == rational( 7, 11));
    CHECK((rational(-49, 121) /= rational( 7, 11)) == rational(-7, 11));

    CHECK((rational(-7, 11) += 0) == rational(-7, 11));
    CHECK((rational(-7, 11) += 7) == rational( 70, 11));
    CHECK((rational( 7, 11) += 7) == rational( 84, 11));
    CHECK((rational(-7, 11) +=-7) == rational(-84, 11));

    CHECK((rational(-7, 11) -= 0) == rational(-7, 11));
    CHECK((rational(-7, 11) -= 7) == rational(-84, 11));
    CHECK((rational( 7, 11) -= 7) == rational(-70, 11));
    CHECK((rational(-7, 11) -=-7) == rational( 70, 11));

    CHECK((rational(-7, 11) *= 0) == rational(0));
    CHECK((rational(-7, 11) *= 7) == rational(-49, 11));
    CHECK((rational( 7, 11) *= 7) == rational( 49, 11));
    CHECK((rational(-7, 11) *=-7) == rational( 49, 11));

    CHECK((rational(-7, 11) *= 7) == rational(-49, 11));
    CHECK((rational( 7, 11) *= 7) == rational( 49, 11));
    CHECK((rational(-7, 11) *=-7) == rational( 49, 11));

    CHECK((rational(0) /= -7) == rational(0));
    CHECK((rational(-7, 11) /= 11) == rational(-7, 121));
    CHECK((rational(-7, 11) /=-11) == rational( 7, 121));
    CHECK((rational(-7, 11) /= 11) == rational(-7, 121));

    CHECK((rational(-7, 11) + rational(0)) == rational(-7, 11));
    CHECK((rational(-7, 11) + rational( 7, 11)) == rational(0));
    CHECK((rational( 7, 11) + rational( 7, 11)) == rational( 14, 11));
    CHECK((rational(-7, 11) + rational(-7, 11)) == rational(-14, 11));

    CHECK((rational(-7, 11) - rational(0)) == rational(-7, 11));
    CHECK((rational(-7, 11) - rational(-7, 11)) == rational(0));
    CHECK((rational( 7, 11) - rational(-7, 11)) == rational( 14, 11));
    CHECK((rational(-7, 11) - rational( 7, 11)) == rational(-14, 11));

    CHECK((rational(-7, 11) * rational(0)) == rational(0));
    CHECK((rational(-7, 11) * rational(-7, 11)) == rational( 49, 121));
    CHECK((rational( 7, 11) * rational(-7, 11)) == rational(-49, 121));
    CHECK((rational(-7, 11) * rational( 7, 11)) == rational(-49, 121));

    CHECK((rational(0) / rational(-7, 11)) == rational(0));
    CHECK((rational( 49, 121) / rational(-7, 11)) == rational(-7, 11));
    CHECK((rational(-49, 121) / rational(-7, 11)) == rational( 7, 11));
    CHECK((rational(-49, 121) / rational( 7, 11)) == rational(-7, 11));

    CHECK((rational(-7, 11) + 0) == rational(-7, 11));
    CHECK((rational(-7, 11) + 7) == rational( 70, 11));
    CHECK((rational( 7, 11) + 7) == rational( 84, 11));
    CHECK((rational(-7, 11) +-7) == rational(-84, 11));

    CHECK((rational(-7, 11) - 0) == rational(-7, 11));
    CHECK((rational(-7, 11) - 7) == rational(-84, 11));
    CHECK((rational( 7, 11) - 7) == rational(-70, 11));
    CHECK((rational(-7, 11) - -7) == rational( 70, 11));

    CHECK((rational(-7, 11) * 0) == rational(0));
    CHECK((rational(-7, 11) * 7) == rational(-49, 11));
    CHECK((rational( 7, 11) * 7) == rational( 49, 11));
    CHECK((rational(-7, 11) *-7) == rational( 49, 11));

    CHECK((rational(-7, 11) *= 7) == rational(-49, 11));
    CHECK((rational( 7, 11) *= 7) == rational( 49, 11));
    CHECK((rational(-7, 11) *=-7) == rational( 49, 11));

    CHECK((rational(0) /= -7) == rational(0));
    CHECK((rational(-7, 11) /= 11) == rational(-7, 121));
    CHECK((rational(-7, 11) /=-11) == rational( 7, 121));
    CHECK((rational(-7, 11) /= 11) == rational(-7, 121));

    CHECK(( 0 + rational(-7, 11)) == rational(-7, 11));
    CHECK(( 7 + rational(-7, 11)) == rational( 70, 11));
    CHECK(( 7 + rational( 7, 11)) == rational( 84, 11));
    CHECK((-7 + rational(-7, 11)) == rational(-84, 11));

    CHECK(( 0 - rational(-7, 11)) == rational( 7, 11));
    CHECK(( 7 - rational(-7, 11)) == rational( 84, 11));
    CHECK(( 7 - rational( 7, 11)) == rational( 70, 11));
    CHECK((-7 - rational(-7, 11)) == rational(-70, 11));

    CHECK(( 0 * rational(-7, 11)) == rational(0));
    CHECK(( 7 * rational(-7, 11)) == rational(-49, 11));
    CHECK(( 7 * rational( 7, 11)) == rational( 49, 11));
    CHECK((-7 * rational(-7, 11)) == rational( 49, 11));

    CHECK(( 0 / rational(-7, 11)) == rational(0));
    CHECK(( 7 / rational(-7, 11)) == rational(-11));
    CHECK(( 7 / rational( 7, 11)) == rational( 11));
    CHECK((-7 / rational(-7, 11)) == rational( 11));
}

TEST_CASE("Sort algorithm") {
    {
        rational rationals[] = {
              rational(1, 2)
            , rational(1, 3)
            , rational(1, 4)
            , rational(1, 5)
            , rational(1, 6)
        };

        std::sort(rationals
                , rationals + sizeof(rationals)/sizeof(rationals[0])
                , std::less<rational>());

        CHECK(rationals[0] == rational(1, 6));
        CHECK(rationals[1] == rational(1, 5));
        CHECK(rationals[2] == rational(1, 4));
        CHECK(rationals[3] == rational(1, 3));
        CHECK(rationals[4] == rational(1, 2));

        std::sort(rationals
                , rationals + sizeof(rationals)/sizeof(rationals[0])
                , std::greater<rational>());

        CHECK(rationals[0] == rational(1, 2));
        CHECK(rationals[1] == rational(1, 3));
        CHECK(rationals[2] == rational(1, 4));
        CHECK(rationals[3] == rational(1, 5));
        CHECK(rationals[4] == rational(1, 6));
    }
}

TEST_CASE("Floor") {
    CHECK(floor(rational(-4, 2)) == -2);
    CHECK(floor(rational(-3, 2)) == -2);
    CHECK(floor(rational(-2, 2)) == -1);
    CHECK(floor(rational(-1, 2)) == -1);
    CHECK(floor(rational( 0, 2)) == 0);
    CHECK(floor(rational( 1, 2)) == 0);
    CHECK(floor(rational( 2, 2)) == 1);
    CHECK(floor(rational( 3, 2)) == 1);
    CHECK(floor(rational( 4, 2)) == 2);
    CHECK(floor(rational( 5, 2)) == 2);
}

TEST_CASE("Ceil") {
    CHECK(ceil(rational(-4, 2)) == -2);
    CHECK(ceil(rational(-3, 2)) == -1);
    CHECK(ceil(rational(-2, 2)) == -1);
    CHECK(ceil(rational(-1, 2)) ==  0);
    CHECK(ceil(rational( 0, 2)) ==  0);
    CHECK(ceil(rational( 1, 2)) ==  1);
    CHECK(ceil(rational( 2, 2)) ==  1);
    CHECK(ceil(rational( 3, 2)) ==  2);
    CHECK(ceil(rational( 4, 2)) ==  2);
    CHECK(ceil(rational( 5, 2)) ==  3);
}

TEST_CASE("Exceptions") {
    CHECK_THROWS(rational(1, 0));
    CHECK_THROWS(pfs::rational<signed char>(std::ratio<1000, 1000>{}));
    CHECK_THROWS(rational(0, 1).invert());
    CHECK_THROWS((rational(1) /= rational(0, 1)));
    CHECK_THROWS((rational(1) /= 0));
}
