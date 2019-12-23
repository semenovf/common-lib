////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 Vladislav Trifochkin
//
// This file is part of [pfs-common](https://github.com/semenovf/pfs-common) library.
//
// Changelog:
//      2019.12.17 Initial version (inspired from https://github.com/mpark/variant)
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/safeformat.hpp"
#include "pfs/fmt.hpp"
#include <chrono>

#ifdef HAVE_QT_CORE
#   include <QString>
#endif

//
// This tests based on loki::SafeFormat tests
//

//
// https://www.reddit.com/r/cpp_questions/comments/40f6xn/timer_using_stdchronosteady_clock/
//
template<typename period = std::micro, typename rep = size_t>
class stopwatch
{
    std::chrono::time_point<std::chrono::high_resolution_clock> begin, end;

public:
    using count_type = std::chrono::duration<rep, period>;

    stopwatch ()
    {
        begin = std::chrono::high_resolution_clock::time_point::min();
        end = begin;
    }

    inline void start ()
    {
        begin = std::chrono::high_resolution_clock::now();
    }

    inline void stop ()
    {
        end = std::chrono::high_resolution_clock::now();
    }

    inline rep count ()
    {
        using namespace std::chrono;
        return duration_cast<count_type>(end - begin).count();
    }
};

template <typename T, typename StringType>
bool testCase (char const * fmt1, std::string const & fmt2, T value)
{
    using string_type = StringType;

#if defined(_WIN32) || defined(_WIN64)
    using fmt = pfs::safeformat<pfs::safeformat_compat_msc>;
#else
    using fmt = pfs::safeformat;
#endif

    char buf[5020];
    string_type s = fmt(string_type(fmt1)).arg(value).str();

#if defined(_WIN32) || defined(_WIN64)
    int i2 =_snprintf(buf, sizeof(buf), fmt2.c_str(), value);
#else
    int i2 = snprintf(buf, sizeof(buf), fmt2.c_str(), value);
#endif

    if (s.size() != static_cast<size_t>(i2) || s != string_type(buf)) {
        std::cout << "\nReference : " << i2 << "; Actual: " << s.size() << ", Difference = " << i2 - int(s.size())
                  << "\nvalue            : [" << value << "]\n"
                  <<   "safeformat format: [" << string_type(fmt1) << "]\n"
                  <<   "snprintf format  : [" << fmt2 << "]\n"
                  <<   "expected result  : [" << buf << "]\n"
                  <<   "actual result    : [" << s << "]\n";
        return false;
    }
    return true;
}

template <typename T, typename StringType>
void testCaseIntegral (std::string const & length_modifier)
{
    using string_type = StringType;

    REQUIRE((testCase<T, string_type>("%o" , std::string("%" ) + length_modifier + 'o', std::numeric_limits<T>::min())));
    REQUIRE((testCase<T, string_type>("%+o", std::string("%+") + length_modifier + 'o', std::numeric_limits<T>::min())));
    REQUIRE((testCase<T, string_type>("%o" , std::string("%" ) + length_modifier + 'o', std::numeric_limits<T>::max())));
    REQUIRE((testCase<T, string_type>("%+o", std::string("%+") + length_modifier + 'o', std::numeric_limits<T>::max())));
    REQUIRE((testCase<T, string_type>("%o" , std::string("%" ) + length_modifier + 'o', 0)));
    REQUIRE((testCase<T, string_type>("%+o", std::string("%+") + length_modifier + 'o', 0)));

    REQUIRE((testCase<T, string_type>("%x" , std::string("%" ) + length_modifier + 'x', std::numeric_limits<T>::min())));
    REQUIRE((testCase<T, string_type>("%+x", std::string("%+") + length_modifier + 'x', std::numeric_limits<T>::min())));
    REQUIRE((testCase<T, string_type>("%x" , std::string("%" ) + length_modifier + 'x', std::numeric_limits<T>::max())));
    REQUIRE((testCase<T, string_type>("%+x", std::string("%+") + length_modifier + 'x', std::numeric_limits<T>::max())));
    REQUIRE((testCase<T, string_type>("%x" , std::string("%" ) + length_modifier + 'x', 0)));
    REQUIRE((testCase<T, string_type>("%+x", std::string("%+") + length_modifier + 'x', 0)));

    REQUIRE((testCase<T, string_type>("%d" , std::string("%" ) + length_modifier + 'd', std::numeric_limits<T>::min())));
    REQUIRE((testCase<T, string_type>("%+d", std::string("%+") + length_modifier + 'd', std::numeric_limits<T>::min())));
    REQUIRE((testCase<T, string_type>("%d" , std::string("%" ) + length_modifier + 'd', std::numeric_limits<T>::max())));
    REQUIRE((testCase<T, string_type>("%+d", std::string("%+") + length_modifier + 'd', std::numeric_limits<T>::max())));
    REQUIRE((testCase<T, string_type>("%d" , std::string("%" ) + length_modifier + 'd', 0)));
    REQUIRE((testCase<T, string_type>("%+d", std::string("%+") + length_modifier + 'd', 0)));

    REQUIRE((testCase<T, string_type>("%i" , std::string("%" ) + length_modifier + 'i', std::numeric_limits<T>::min())));
    REQUIRE((testCase<T, string_type>("%+i", std::string("%+") + length_modifier + 'i', std::numeric_limits<T>::min())));
    REQUIRE((testCase<T, string_type>("%i" , std::string("%" ) + length_modifier + 'i', std::numeric_limits<T>::max())));
    REQUIRE((testCase<T, string_type>("%+i", std::string("%+") + length_modifier + 'i', std::numeric_limits<T>::max())));
    REQUIRE((testCase<T, string_type>("%i" , std::string("%" ) + length_modifier + 'i', 0)));
    REQUIRE((testCase<T, string_type>("%+i", std::string("%+") + length_modifier + 'i', 0)));

    REQUIRE((testCase<T, string_type>("%u" , std::string("%" ) + length_modifier + 'u', std::numeric_limits<T>::min())));
    REQUIRE((testCase<T, string_type>("%+u", std::string("%+") + length_modifier + 'u', std::numeric_limits<T>::min())));
    REQUIRE((testCase<T, string_type>("%u" , std::string("%" ) + length_modifier + 'u', std::numeric_limits<T>::max())));
    REQUIRE((testCase<T, string_type>("%+u", std::string("%+") + length_modifier + 'u', std::numeric_limits<T>::max())));
    REQUIRE((testCase<T, string_type>("%u" , std::string("%" ) + length_modifier + 'u', 0)));
    REQUIRE((testCase<T, string_type>("%+u", std::string("%+") + length_modifier + 'u', 0)));
}

template <typename T, typename StringType>
void testCaseFloat (std::string const & length_modifier)
{
    using string_type = StringType;

    REQUIRE((testCase<T, string_type>("%e", std::string("%") + length_modifier + 'e', std::numeric_limits<T>::min())));
    REQUIRE((testCase<T, string_type>("%e", std::string("%") + length_modifier + 'e', std::numeric_limits<T>::max())));
    REQUIRE((testCase<T, string_type>("%E", std::string("%") + length_modifier + 'E', std::numeric_limits<T>::min())));
    REQUIRE((testCase<T, string_type>("%E", std::string("%") + length_modifier + 'E', std::numeric_limits<T>::max())));
    REQUIRE((testCase<T, string_type>("%f", std::string("%") + length_modifier + 'f', std::numeric_limits<T>::min())));
    REQUIRE((testCase<T, string_type>("%f", std::string("%") + length_modifier + 'f', std::numeric_limits<T>::max())));
    REQUIRE((testCase<T, string_type>("%g", std::string("%") + length_modifier + 'g', std::numeric_limits<T>::min())));
    REQUIRE((testCase<T, string_type>("%g", std::string("%") + length_modifier + 'g', std::numeric_limits<T>::max())));
    REQUIRE((testCase<T, string_type>("%G", std::string("%") + length_modifier + 'G', std::numeric_limits<T>::min())));
    REQUIRE((testCase<T, string_type>("%G", std::string("%") + length_modifier + 'G', std::numeric_limits<T>::max())));
}

template <typename StringType>
void testCaseString ()
{
    using string_type = StringType;
    REQUIRE((testCase<char const *, string_type>("%s", "%s", "Hello")));
}

template <typename StringType>
void test0 ()
{
    using string_type = StringType;

    testCaseIntegral<char          , string_type>(pfs::printf_length_modifier<char>::value());
    testCaseIntegral<signed char   , string_type>(pfs::printf_length_modifier<signed char>::value());
    testCaseIntegral<unsigned char , string_type>(pfs::printf_length_modifier<unsigned char>::value());
    testCaseIntegral<short         , string_type>(pfs::printf_length_modifier<short>::value());
    testCaseIntegral<unsigned short, string_type>(pfs::printf_length_modifier<unsigned short>::value());
    testCaseIntegral<int           , string_type>(pfs::printf_length_modifier<int>::value());
    testCaseIntegral<unsigned int  , string_type>(pfs::printf_length_modifier<unsigned int>::value());
    testCaseIntegral<long          , string_type>(pfs::printf_length_modifier<long>::value());
    testCaseIntegral<unsigned long , string_type>(pfs::printf_length_modifier<unsigned long>::value());
    testCaseIntegral<long long, string_type>(pfs::printf_length_modifier<long long>::value());
    testCaseIntegral<unsigned long long, string_type>(pfs::printf_length_modifier<unsigned long long>::value());
    testCaseFloat<float, string_type>(pfs::printf_length_modifier<float>::value());
    testCaseFloat<double, string_type>(pfs::printf_length_modifier<double>::value());
    testCaseFloat<long double, string_type>(pfs::printf_length_modifier<long double>::value());
    testCaseString<string_type>();
}

template <typename StringType>
void test1 ()
{
    using string_type = StringType;

    REQUIRE((testCase<void *  , string_type>("%p" , "%p", reinterpret_cast<void *>(0xabcd))));
    REQUIRE((testCase<int *   , string_type>("%p" , "%p", reinterpret_cast<int *>(0xabcd))));
    REQUIRE((testCase<long *  , string_type>("%p" , "%p", reinterpret_cast<long *>(0xabcd))));
    REQUIRE((testCase<double *, string_type>("%p" , "%p", reinterpret_cast<double *>(0xabcd))));
}

template <class Integral1, class Integral2>
Integral2 randomInt (Integral1 low, Integral2 up)
{
    // From ``Accelerated C++'', page 135:
    // random integer in the range [0, n)
    // We adjust to generate in the range [0, n]
    const Integral2
    low2 = low, n = up - low;

    REQUIRE(n > 0);

    const size_t bucket_size = RAND_MAX / n;
    REQUIRE(bucket_size > 0);

    Integral2 r;
    do
        r = Integral2(rand() / bucket_size);
    while (r > n);

    r = r + low2;
    REQUIRE(r >= low2);
    REQUIRE(r <= up);
    return r;
}

std::string randomString (unsigned int maxSize)
{
    size_t i = 0;
    size_t n = randomInt(0, maxSize);
    std::string result;

    for (; i < n; ++i) {
        result.push_back(char(randomInt('a', 'z')));
    }

    return result;
}

template <typename StringType>
void test2 (unsigned limit)
{
    typedef StringType string_type;

    srand(0);

    for (unsigned i = 0; i < limit; ++i) {
        // Generate a random string for the head
        std::string lead = randomString(100);

        // This string will hold a random format specification
        std::string formatSpec(lead + "|%");

        // Generate a random set of flags
        static std::string const flags("-+0 #");
        size_t maxFlags = randomInt(0u, flags.length() - 1);

        for (size_t j = 0; j != maxFlags; ++j) {
            formatSpec += flags[randomInt(0u, flags.length() - 1)];
        }

        // Generate an optional random width
        if (randomInt(0, 1)) {
            const unsigned int width = randomInt(0, 100);
            char buf[4];
            sprintf(buf, "%u", width);
            formatSpec += buf;
        }

        // Generate an optional random precision
        if (randomInt(0, 1)) {
            const unsigned int prec = randomInt(0, 100);
            char buf[4];
            sprintf(buf, "%u", prec);
            formatSpec += '.';
            formatSpec += buf;
        }

        // Generate a random type character
        static std::string const type("cdeEfgGiopsuxX");
        char typeSpec = type.at(randomInt(0u, type.length() - 1));

        // Generate an optional type prefix
//            static const string prefix("hl");
//            if (typeSpec != 's' && randomInt(0, 1)) {
//                formatSpec += prefix[randomInt(0u, prefix.size() - 1)];
//            }

        formatSpec += typeSpec;
        formatSpec += '|';
        formatSpec += randomString(100);

        switch (typeSpec) {
        case 'c':
            CHECK_MESSAGE((testCase<int, string_type>(formatSpec.c_str(), formatSpec, randomInt(1, 127)))
                    , formatSpec.c_str());
            break;
        case 'd':
        case 'i':
        case 'o':
        case 'u':
        case 'x':
        case 'X':
            //TestCase(formatSpec, RandomInt(-10000, 10000));
            // don't test negative values on 64bit systems, because
            // snprintf does not support 64 Bit values
            CHECK_MESSAGE((testCase<int, string_type>(formatSpec.c_str()
                            , formatSpec
                            , randomInt(-10000 * (sizeof(size_t) > 4 ? 0 : 1) , 10000)))
                    , formatSpec.c_str());
            break;
        case 'e':
        case 'E':
        case 'f':
        case 'g':
        case 'G':
            CHECK_MESSAGE((testCase<double, string_type>(formatSpec.c_str()
                            , formatSpec
                            , randomInt(-10000, 10000) / double(randomInt(1, 100))))
                    , formatSpec.c_str());
            break;
        case 'n':
            break;
        case 'p':
            {
                void * p = malloc(randomInt(1, 1000));
                CHECK_MESSAGE((testCase<void *, string_type>(formatSpec.c_str(), formatSpec, p))
                        , formatSpec.c_str());
                free(p);
            }
            break;
        case 's':
            CHECK_MESSAGE((testCase<char const *, string_type>(formatSpec.c_str()
                            , formatSpec
                            , randomString(100).c_str()))
                    , formatSpec.c_str());
            break;
        default:
            REQUIRE(false);
            break;
        }
    }
}

// speed test
//
template <typename StringType>
void test3 ()
{
    using string_type = StringType;

    char buf[512];
    stopwatch<> sw;

    int loop = 100;

    sw.start();
    for (int i = loop; i > 0; --i)
        sprintf(buf, "Hey, %u frobnicators and %u twiddlicators\n", i, i);
    sw.stop();
    auto ellapsed_sprintf = sw.count();

    sw.start();
    for (int i = loop; i > 0; --i)
        std::string s = pfs::safeformat("Hey, %u frobnicators and %u twiddlicators\n")(i)(i);
    sw.stop();
    auto ellapsed_safeformat = sw.count();

    sw.start();
    for (int i = loop; i > 0; --i)
        std::stringstream() << "Hey, " << i << " frobnicators and " << i <<" twiddlicators\n";
    sw.stop();
    auto ellapsed_sstream = sw.count();

    sw.start();
    for (int i = loop; i > 0; --i)
        fmt::format("Hey, {} frobnicators and {} twiddlicators\n", i, i);
    sw.stop();
    auto ellapsed_fmtlib = sw.count();

#ifdef HAVE_QT_CORE
    sw.start();
    for (int i = loop; i > 0; --i) {
        QString s = QString("Hey, %1 frobnicators and %2 twiddlicators\n").arg(i).arg(i);
        (void)s;
    }
    sw.stop();
    auto ellapsed_qstring = sw.count();
#endif

    std::cout << std::endl << "Elapsed time for " << loop << " outputs:"
            << "\n\tsprintf      = " << ellapsed_sprintf
            << "\n\tsafeformat   = " << ellapsed_safeformat
            << "\n\tstringstream = " << ellapsed_sstream
            << "\n\tfmtlib::fmt  = " << ellapsed_fmtlib << std::endl
#ifdef HAVE_QT_CORE
            << "\tQString      = " << ellapsed_qstring << std::endl
#endif
    ;
}

TEST_CASE("SafeFormat") {
    test0<std::string>();
    test1<std::string>();
    test2<std::string>(1000);
    test3<std::string>();

//     if (argc > 1) {
//         test2<pfs::string>(pfs::numeric_limits<unsigned>::max());
//     }
}
