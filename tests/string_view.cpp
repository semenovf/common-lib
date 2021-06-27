////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of [common-lib](https://github.com/semenovf/common-lib) library.
//
// Changelog:
//      2020.06.13 Initial version (inspired from https://github.com/tcbrindle/cpp17_headers)
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/string_view.hpp"
#include <string>
#include <utility>

#if defined(PFS_NO_STD_STRING_VIEW)
using pfs::basic_string_view;
using pfs::string_view;
#else
using std::basic_string_view;
using std::string_view;
#endif

//  Should be equal
void interop (std::string const & str, string_view ref)
{
//  assert ( str == ref );
    CHECK(str.size() == ref.size ());
    CHECK(std::equal( str.begin (),  str.end (),  ref.begin ()));
    CHECK(std::equal( str.rbegin (), str.rend (), ref.rbegin ()));
}

void null_tests (char const * p)
{
//  All zero-length string-refs should be equal
    string_view sr1; // nullptr, 0
    string_view sr2 {nullptr, 0};
    string_view sr3 {p, 0};
    string_view sr4 {p};
    //sr4.clear ();

    CHECK(sr1 == sr2);
    CHECK(sr1 == sr3);
    CHECK(sr2 == sr3);
    //assert ( sr1 == sr4 );
    }

//  make sure that substrings work just like strings
void test_substr (std::string const & str)
{
    const size_t sz = str.size();
    string_view ref {str};

//  Substrings at the end
    for (size_t i = 0; i <= sz; ++ i)
        interop(str.substr(i), ref.substr(i));

//  Substrings at the beginning
    for (size_t i = 0; i <= sz; ++ i)
        interop(str.substr(0, i), ref.substr(0, i));

//  All possible substrings
    for (size_t i = 0; i < sz; ++i)
        for (size_t j = i; j < sz; ++j)
            interop(str.substr(i, j), ref.substr(i, j));
    }

//  make sure that removing prefixes and suffixes work just like strings
void test_remove (std::string const & str)
{
    const size_t sz = str.size();
    std::string work;
    string_view ref;

    for (size_t i = 1; i <= sz; ++i) {
        work = str;
        ref  = str;

        while (ref.size () >= i) {
            interop(work, ref);
            work.erase( 0, i );
            ref.remove_prefix (i);
        }
    }

    for (size_t i = 1; i < sz; ++ i) {
        work = str;
        ref  = str;

        while (ref.size () >= i) {
            interop(work, ref);
            work.erase(work.size() - i, i);
            ref.remove_suffix(i);
        }
    }
}

char const * test_strings[] = {
      ""
    , "1"
    , "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    , "0123456789"
    , nullptr
};

TEST_CASE("basic")
{
    char const ** p = & test_strings[0];

    while (*p != nullptr) {
        interop(*p, *p);
        test_substr(*p);
        test_remove(*p);
        null_tests(*p);

        p++;
    }
}

////////////////////////////////////////////////////////////////////////////////

// NOTE string_view::ends_with since C++20
#if __cplusplus > 201703L || defined(PFS_NO_STD_STRING_VIEW)
void ends_with (const char *arg)
{
    const size_t sz = std::strlen ( arg );
    string_view sr ( arg );
    string_view sr2 ( arg );
    char const * p = arg;

    while (*p) {
        CHECK(sr.ends_with(p));
        ++p;
    }

    while (!sr2.empty ()) {
        CHECK(sr.ends_with (sr2));
        sr2.remove_prefix(1);
    }

    sr2 = arg;

    while (!sr2.empty()) {
        CHECK(sr.ends_with(sr2));
        sr2.remove_prefix(1);
    }

    char ch = sz == 0 ? '\0' : arg [ sz - 1 ];
    sr2 = arg;

    if (sz > 0)
        CHECK(sr2.ends_with(ch));

    CHECK(!sr2.ends_with(++ch));
    CHECK(sr2.ends_with(string_view()));
}
#endif

// NOTE string_view::starts_with since C++20
#if __cplusplus > 201703L || defined(PFS_NO_STD_STRING_VIEW)
void starts_with (char const * arg)
{
    const size_t sz = std::strlen(arg);
    string_view sr {arg};
    string_view sr2 {arg};

    char const * p = arg + std::strlen(arg) - 1;

    while (p >= arg) {
        std::string foo {arg, p + 1};
        CHECK(sr.starts_with(foo));
        --p;
    }

    while (!sr2.empty()) {
        CHECK(sr.starts_with(sr2));
        sr2.remove_suffix(1);
    }

    char ch = *arg;
    sr2 = arg;

    if ( sz > 0 ) {
        CHECK(sr2.starts_with(ch));
        CHECK(!sr2.starts_with(++ch));
        CHECK(sr2.starts_with(string_view()));
    }
}
#endif

void reverse (char const * arg)
{
//  Round trip
    string_view sr1 {arg};
    std::string string1 {sr1.rbegin(), sr1.rend()};
    string_view sr2 {string1};
    std::string string2 {sr2.rbegin(), sr2.rend()};

    CHECK(std::equal(sr2.rbegin(), sr2.rend(), arg));
    CHECK(string2 == arg);
    CHECK(std::equal(sr1.begin(), sr1.end(), string2.begin()));
}

//  This helper function eliminates signed vs. unsigned warnings
string_view::size_type ptr_diff (char const * res, char const * base)
{
    CHECK(res >= base);
    return static_cast<string_view::size_type>(res - base);
}

void find (char const * arg)
{
    string_view sr1;
    string_view sr2;
    const char *p;

//  Look for each character in the string(searching from the start)
    p = arg;
    sr1 = arg;

    while ( *p ) {
        string_view::size_type pos = sr1.find(*p);
        CHECK((pos != string_view::npos && (pos <= ptr_diff(p, arg))));
        ++p;
    }

//  Look for each character in the string (searching from the end)
    p = arg;
    sr1 = arg;

    while (*p) {
        string_view::size_type pos = sr1.rfind(*p);
        CHECK((pos != string_view::npos && pos < sr1.size() && (pos >= ptr_diff(p, arg))));
        ++p;
    }

//  Look for pairs on characters (searching from the start)
    sr1 = arg;
    p = arg;

    while (*p && *(p+1)) {
        string_view sr3 {p, 2};
        string_view::size_type pos = sr1.find(sr3);
        CHECK((pos != string_view::npos && pos <= static_cast<string_view::size_type>(p - arg)));
        p++;
    }

    sr1 = arg;
    p = arg;
//  for all possible chars, see if we find them in the right place.
//  Note that strchr will/might do the _wrong_ thing if we search for nullptr
    for (int ch = 1; ch < 256; ++ch) {
        string_view::size_type pos = sr1.find(ch);
        const char *strp = std::strchr ( arg, ch );
        CHECK((strp == nullptr) == (pos == string_view::npos));

        if (strp != nullptr)
            CHECK(ptr_diff(strp, arg) == pos);
    }

    sr1 = arg;
    p = arg;
    //  for all possible chars, see if we find them in the right place.
    //  Note that strchr will/might do the _wrong_ thing if we search for nullptr
    for (int ch = 1; ch < 256; ++ch) {
        string_view::size_type pos = sr1.rfind(ch);
        const char *strp = std::strrchr ( arg, ch );
        CHECK((strp == nullptr ) == (pos == string_view::npos));

        if (strp != nullptr)
            CHECK(ptr_diff ( strp, arg ) == pos);
    }

    //  Find everything at the start
    p = arg;
    sr1 = arg;

    while (!sr1.empty()) {
        string_view::size_type pos = sr1.find(*p);
        CHECK(pos == 0);
        sr1.remove_prefix (1);
        ++p;
    }

    //  Find everything at the end
    sr1  = arg;
    p    = arg + std::strlen ( arg ) - 1;
    while (!sr1.empty()) {
        string_view::size_type pos = sr1.rfind(*p);
        CHECK(pos == sr1.size () - 1);
        sr1.remove_suffix (1);
        --p;
    }

    //  Find everything at the start
    sr1  = arg;
    p    = arg;
    while ( !sr1.empty ()) {
        string_view::size_type pos = sr1.find_first_of(*p);
        CHECK(pos == 0);
        sr1.remove_prefix (1);
        ++p;
    }


    //  Find everything at the end
    sr1  = arg;
    p    = arg + std::strlen ( arg ) - 1;
    while (!sr1.empty ()) {
        string_view::size_type pos = sr1.find_last_of(*p);
        CHECK(pos == sr1.size () - 1);
        sr1.remove_suffix (1);
        --p;
    }

    //  Basic sanity checking for "find_first_of / find_first_not_of"
    sr1 = arg;
    sr2 = arg;
    while (!sr1.empty()) {
        CHECK(sr1.find_first_of(sr2) == 0);
        CHECK(sr1.find_first_not_of(sr2) == string_view::npos);
        sr1.remove_prefix ( 1 );
    }

    p = arg;
    sr1 = arg;
    while (*p) {
        string_view::size_type pos1 = sr1.find_first_of(*p);
        string_view::size_type pos2 = sr1.find_first_not_of(*p);
        CHECK((pos1 != string_view::npos && pos1 < sr1.size() && pos1 <= ptr_diff(p, arg)));

        if (pos2 != string_view::npos) {
            for (size_t i = 0 ; i < pos2; ++i)
                CHECK(sr1[i] == *p);

            CHECK(sr1 [ pos2 ] != *p);
        }

        CHECK(pos2 != pos1);
        ++p;
    }

//  Basic sanity checking for "find_last_of / find_last_not_of"
    sr1 = arg;
    sr2 = arg;
    while (!sr1.empty()) {
        CHECK(sr1.find_last_of(sr2)     == (sr1.size() - 1));
        CHECK(sr1.find_last_not_of(sr2) == string_view::npos);
        sr1.remove_suffix ( 1 );
    }

    p = arg;
    sr1 = arg;
    while (*p) {
        string_view::size_type pos1 = sr1.find_last_of(*p);
        string_view::size_type pos2 = sr1.find_last_not_of(*p);
        CHECK((pos1 != string_view::npos && pos1 < sr1.size() && pos1 >= ptr_diff(p, arg)));
        CHECK((pos2 == string_view::npos || pos1 < sr1.size()));

        if (pos2 != string_view::npos) {
            for (size_t i = sr1.size() -1 ; i > pos2; --i)
                CHECK(sr1[i] == *p);
            CHECK(sr1[pos2] != *p);
        }

        CHECK(pos2 != pos1);
        ++p;
    }
}

template <typename T>
class custom_allocator
{
public:
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef void* void_pointer;
    typedef const void* const_void_pointer;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef T& reference;
    typedef const T& const_reference;

    template<class U>
    struct rebind {
        typedef custom_allocator<U> other;
    };

    custom_allocator () noexcept {}
    template <typename U>
    custom_allocator (custom_allocator<U> const&) noexcept {}

    pointer allocate (size_type n) const
    {
        return static_cast<pointer>(std::malloc(sizeof(value_type) * n));
    }

    void deallocate (pointer p, size_type) const noexcept
    {
        std::free(p);
    }

    pointer address (reference value) const noexcept
    {
        return & value;
    }

    const_pointer address (const_reference value) const noexcept
    {
        return & value;
    }

    constexpr size_type max_size () const noexcept
    {
        return (~(size_type)0u) / sizeof(value_type);
    }

// #if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
// #if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)
    template <class U, class... Args>
    void construct (U * ptr, Args &&... args) const
    {
        ::new((void*)ptr) U(static_cast<Args&&>(args)...);
    }
// #else
//     template <class U, class V>
//     void construct(U* ptr, V&& value) const {
//         ::new((void*)ptr) U(static_cast<V&&>(value));
//         }
// #endif
// #else
//     template <class U, class V>
//     void construct(U* ptr, const V& value) const {
//         ::new((void*)ptr) U(value);
//         }
// #endif

    template <class U>
    void construct (U * ptr) const
    {
        ::new((void*)ptr) U();
    }

    template <class U>
    void destroy (U * ptr) const {
        (void)ptr;
        ptr->~U();
    }
};

template <typename T, typename U>
constexpr bool operator == (const custom_allocator<T> &, const custom_allocator<U> &) noexcept
{
    return true;
}

template <typename T, typename U>
constexpr bool operator != (const custom_allocator<T> &, const custom_allocator<U> &) noexcept
{
    return false;
}

// NOTE Experimental feature yet (C++20)
#if defined(PFS_NO_STD_STRING_VIEW)
void to_string (char const * arg)
{
    string_view sr1;
    std::string str1;
    std::string str2;

    str1.assign(arg);
    sr1 = arg;
//  str2 = sr1.to_string<std::allocator<char> > ();
    str2 = sr1.to_string ();
    CHECK(str1 == str2);

    std::basic_string<char, std::char_traits<char>, custom_allocator<char> > str3 = sr1.to_string(custom_allocator<char>());
    CHECK(std::strcmp(str1.c_str(), str3.c_str()) == 0);

//#ifndef BOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS
    std::string str4 = static_cast<std::string>(sr1);
    CHECK(str1 == str4);
//#endif
}
#endif

void compare (char const * arg)
{
    string_view sr1;
    std::string str1;
    std::string str2 = str1;

    str1.assign(arg);
    sr1 = arg;
    CHECK(sr1  == sr1);   // compare string_view and string_view
    CHECK(sr1  == str1);  // compare string and string_view
    CHECK(str1 == sr1);   // compare string_view and string
    CHECK(sr1  == arg);   // compare string_view and pointer
    CHECK(arg  == sr1);   // compare pointer and string_view

    if (sr1.size () > 0 ) {
        (*str1.rbegin())++;
        CHECK(sr1  != str1);
        CHECK(str1 != sr1);
        CHECK(sr1 < str1);
        CHECK(sr1 <= str1);
        CHECK(str1 > sr1);
        CHECK(str1 >= sr1);

        (*str1.rbegin()) -= 2;
        CHECK(sr1  != str1);
        CHECK(str1 != sr1);
        CHECK(sr1 > str1);
        CHECK(sr1 >= str1);
        CHECK(str1 < sr1);
        CHECK(str1 <= sr1);
    }
}

const char * test_strings1[] = {
    "",
    "0",
    "abc",
    "AAA",  // all the same
    "adsfadadiaef;alkdg;aljt;j agl;sjrl;tjs;lga;lretj;srg[w349u5209dsfadfasdfasdfadsf",
    "abc\0asdfadsfasf",
    nullptr
    };

// NOTE string_view::starts_with(), ends_with() since C++20
// besides to_string() is experimental feature yet (C++20)
#if defined(PFS_NO_STD_STRING_VIEW)
TEST_CASE("test_main")
{
    char const ** p = & test_strings1[0];

    while (*p != nullptr) {
        starts_with(*p);
        ends_with(*p);
        reverse(*p);
        find(*p);
        to_string(*p);
        compare(*p);

        p++;
    }
}
#endif

////////////////////////////////////////////////////////////////////////////////
// Test IO
////////////////////////////////////////////////////////////////////////////////
static char const * test_strings2[] = {
      "begin"
    , "abcd"
    , "end"
};

//! The context with test data for particular character type
template <typename CharT>
struct context
{
    typedef CharT char_type;
    typedef std::basic_string <char_type> string_type;
    typedef std::basic_ostringstream <char_type> ostream_type;

    string_type begin, abcd, end;

    context ()
    {
        string_view str = test_strings2[0];
        std::copy(str.begin(), str.end(), std::back_inserter(begin));

        str = test_strings2[1];
        std::copy(str.begin(), str.end(), std::back_inserter(abcd));

        str = test_strings2[2];
        std::copy(str.begin(), str.end(), std::back_inserter(end));
    }
};

// Test regular output
template <typename CharT>
void string_view_output ()//, CharT, char_types)
{
    using char_type = CharT;
    using ostream_type = std::basic_ostringstream<char_type>;
    using string_view_type = basic_string_view<char_type>;

    context<char_type> ctx;

    ostream_type strm;
    strm << string_view_type(ctx.abcd);
    CHECK(strm.str() == ctx.abcd);
}

// Test support for padding
template <typename CharT>
void padding ()
{
    using char_type = CharT;
    using ostream_type = std::basic_ostringstream<char_type>;
    using string_view_type = basic_string_view<char_type>;

    context<char_type> ctx;

    // Test for padding
    {
        ostream_type strm_ref;
        strm_ref << ctx.begin << std::setw(8) << string_view_type(ctx.abcd) << ctx.end;

        ostream_type strm_correct;
        strm_correct << ctx.begin << std::setw(8) << ctx.abcd << ctx.end;

        CHECK(strm_ref.str() == strm_correct.str());
    }

    // Test for long padding
    {
        ostream_type strm_ref;
        strm_ref << ctx.begin << std::setw(100) << string_view_type(ctx.abcd) << ctx.end;

        ostream_type strm_correct;
        strm_correct << ctx.begin << std::setw(100) << ctx.abcd << ctx.end;

        CHECK(strm_ref.str() == strm_correct.str());
    }

    // Test that short width does not truncate the string
    {
        ostream_type strm_ref;
        strm_ref << ctx.begin << std::setw(1) << string_view_type(ctx.abcd) << ctx.end;

        ostream_type strm_correct;
        strm_correct << ctx.begin << std::setw(1) << ctx.abcd << ctx.end;

        CHECK(strm_ref.str() == strm_correct.str());
    }
}

// Test support for padding fill
template <typename CharT>
void padding_fill ()
{
    using char_type = CharT;
    using ostream_type = std::basic_ostringstream<char_type>;
    using string_view_type = basic_string_view<char_type>;

    context<char_type> ctx;

    ostream_type strm_ref;
    strm_ref << ctx.begin << std::setfill(static_cast< char_type >('x'))
        << std::setw(8) << string_view_type(ctx.abcd) << ctx.end;

    ostream_type strm_correct;
    strm_correct << ctx.begin << std::setfill(static_cast< char_type >('x'))
        << std::setw(8) << ctx.abcd << ctx.end;

    CHECK(strm_ref.str() == strm_correct.str());
}

// Test support for alignment
template <typename CharT>
void alignment ()
{
//     typedef CharT char_type;
//     typedef std::basic_ostringstream< char_type > ostream_type;
//     typedef boost::basic_string_view< char_type > string_view_type;
    using char_type = CharT;
    using ostream_type = std::basic_ostringstream<char_type>;
    using string_view_type = basic_string_view<char_type>;

    context<char_type> ctx;

    // Left alignment
    {
        ostream_type strm_ref;
        strm_ref << ctx.begin << std::left << std::setw(8)
            << string_view_type(ctx.abcd) << ctx.end;

        ostream_type strm_correct;
        strm_correct << ctx.begin << std::left << std::setw(8)
            << ctx.abcd << ctx.end;

        CHECK(strm_ref.str() == strm_correct.str());
    }

    // Right alignment
    {
        ostream_type strm_ref;
        strm_ref << ctx.begin << std::right << std::setw(8)
            << string_view_type(ctx.abcd) << ctx.end;

        ostream_type strm_correct;
        strm_correct << ctx.begin << std::right << std::setw(8)
            << ctx.abcd << ctx.end;

        CHECK(strm_ref.str() == strm_correct.str());
    }
}

TEST_CASE("test_io")
{
    string_view_output<char>();
    string_view_output<wchar_t>();
    string_view_output<char16_t>();
    string_view_output<char32_t>();

    padding<char>();
    padding<wchar_t>();
    
#if !defined(PFS_NO_STD_STRING_VIEW)    
    padding<char16_t>();
    padding<char32_t>();
#endif    

    padding_fill<char>();
    padding_fill<wchar_t>();
    
#if !defined(PFS_NO_STD_STRING_VIEW)        
//     padding_fill<char16_t>(); // Throws exception std::bad_cast
//     padding_fill<char32_t>(); // Throws exception std::bad_cast
#endif

    alignment<char>();
    alignment<wchar_t>();
    
#if !defined(PFS_NO_STD_STRING_VIEW)        
    alignment<char16_t>();
    alignment<char32_t>();
#endif
}
