////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 Vladislav Trifochkin
//
// This file is part of [pfs-modulus](https://github.com/semenovf/pfs-modulus) library.
//
// Changelog:
//      2019.12.19 Initial version (inhereted from https://github.com/semenovf/pfs)
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/fmt.hpp"
#include "pfs/filesystem.hpp"

#if (defined(_WIN32) || defined(_WIN64)) && defined(_UNICODE)
#   define _STR(str) L##str
#else
#   define _STR(str) str
#endif

TEST_CASE("Filesystem path") {
#if defined(PFS_NO_STD_FILESYSTEM)
    namespace fs = pfs::filesystem;
#else
    namespace fs = std::filesystem;
#endif

////////////////////////////////////////////////////////////////////////////////
// Constructors                                                               //
////////////////////////////////////////////////////////////////////////////////
    {
        fs::path p_from_cstr("/usr/lib/sendmail.cf");
        fs::path p_from_pfs_string(std::string("/usr/lib/sendmail.cf"));

        fs::path p1 = "/usr/lib/sendmail.cf"; // portable format
        fs::path p2 = "C:\\users\\abcdef\\AppData\\Local\\Temp\\"; // native format
        fs::path p3 = _STR("D:/猫.txt");           // wide string

        CHECK(p1 == fs::path::string_type(_STR("/usr/lib/sendmail.cf")));
        CHECK(p2 == fs::path::string_type(_STR("C:\\users\\abcdef\\AppData\\Local\\Temp\\")));
        CHECK(p3 == fs::path::string_type(_STR("D:/猫.txt")));
    }

////////////////////////////////////////////////////////////////////////////////
// Assign operators
////////////////////////////////////////////////////////////////////////////////
    {
        fs::path p = _STR("C:/users/abcdef/AppData/Local");
        p = p / _STR("Temp"); // move assignment

        CHECK(p == fs::path::string_type(_STR("C:/users/abcdef/AppData/Local/Temp")));

        auto str = _STR("D:/猫.txt");
        p = str; // assignment from a source

        CHECK(p == fs::path::string_type(_STR("D:/猫.txt")));
    }

////////////////////////////////////////////////////////////////////////////////
// Concatenation #1
////////////////////////////////////////////////////////////////////////////////
    {
        fs::path p1 {_STR("//host")};
        p1 /= _STR("foo");
        //fmt::print(_STR("*** [{}] ***\n"), p1.c_str());

        // where "//host" is a root-name
#if !defined(PFS_NO_STD_FILESYSTEM)
        CHECK((fs::path(_STR("//host")) /= _STR("foo")) == fs::path::string_type(_STR("//host/foo")));
#else
        CHECK((fs::path(_STR("//host")) /= _STR("foo")) == fs::path::string_type(_STR("//hostfoo"))); // FIXME for GHC version
#endif

        CHECK((fs::path(_STR("//host/")) /= _STR("foo")) == fs::path::string_type(_STR("//host/foo")));

        // Non-member function
#if !defined(PFS_NO_STD_FILESYSTEM)
        CHECK(fs::path(_STR("//host")) / _STR("foo")  == fs::path::string_type(_STR("//host/foo")));
#else
        CHECK(fs::path(_STR("//host")) / _STR("foo")  == fs::path::string_type(_STR("//hostfoo"))); // FIXME for GHC version
#endif

        CHECK(fs::path(_STR("//host/")) / _STR("foo") == fs::path::string_type(_STR("//host/foo"))); // appends without separator

#if defined(_WIN32) || defined(_WIN64)
        CHECK((fs::path("foo") /= "C:/bar") == fs::path::string_type(_STR("C:/bar")));     // the result is "C:/bar" (replaces)
        CHECK((fs::path("foo") /= "C:") == fs::path::string_type(_STR("C:")));             // the result is "C:"     (replaces)
        CHECK((fs::path("C:") /= "") == fs::path::string_type(_STR("C:")));                // the result is "C:"     (appends, without separator)
        CHECK((fs::path("C:foo") /= "/bar") == fs::path::string_type(_STR("C:/bar")));     // yields "C:/bar"        (removes relative path, then appends)
        CHECK((fs::path("C:foo") /= "C:bar") == fs::path::string_type(_STR("C:foo/bar"))); // yields "C:foo/bar"     (appends, omitting p's root-name)

        // Non-member function
        CHECK(fs::path("foo") / "C:/bar" == fs::path::string_type(_STR("C:/bar")));     // the result is "C:/bar" (replaces)
        CHECK(fs::path("foo") / "C:" == fs::path::string_type(_STR("C:")));             // the result is "C:"     (replaces)
        CHECK(fs::path("C:") / "" == fs::path::string_type(_STR("C:")));                // the result is "C:"     (appends, without separator)
        CHECK(fs::path("C:foo") / "/bar" == fs::path::string_type(_STR("C:/bar")));     // yields "C:/bar"        (removes relative path, then appends)
        CHECK(fs::path("C:foo") / "C:bar" == fs::path::string_type(_STR("C:foo/bar"))); // yields "C:foo/bar"     (appends, omitting p's root-name)
#else
        CHECK((fs::path("foo") /= "") == fs::path::string_type(_STR("foo/"))); // the result is "foo/" (appends) // FIXME for GHC version
        CHECK((fs::path("foo") /= "/bar") == fs::path::string_type(_STR("/bar"))); // the result is "/bar" (replaces) // FIXME for GHC version

        // Non-member function
        // FIXME
        CHECK(fs::path("foo") / "" == fs::path::string_type(_STR("foo/"))); // the result is "foo/" (appends) // FIXME for GHC version
        CHECK(fs::path("foo") / "/bar" == fs::path::string_type(_STR("/bar"))); // the result is "/bar" (replaces) // FIXME for GHC version
#endif
    }

////////////////////////////////////////////////////////////////////////////////
// Concatenation #2
////////////////////////////////////////////////////////////////////////////////
    {
        fs::path p; // empty path
        p += _STR("var"); // does not insert a separator

        CHECK(p == fs::path::string_type(_STR("var")));

        p += _STR("lib"); // does not insert a separator

        CHECK(p == fs::path::string_type(_STR("varlib")));
    }

////////////////////////////////////////////////////////////////////////////////
// Modifiers #1
////////////////////////////////////////////////////////////////////////////////
    {
        fs::path p(_STR("/path/to/file"));

        CHECK(!p.empty());

        p.clear();

        CHECK(p.empty());
    }

////////////////////////////////////////////////////////////////////////////////
// Modifiers #2
////////////////////////////////////////////////////////////////////////////////
    {
        // TODO Test path::make_preferred()
    }

////////////////////////////////////////////////////////////////////////////////
// Modifiers #3
////////////////////////////////////////////////////////////////////////////////
    {
        CHECK(fs::path(_STR("foo/bar")).remove_filename() == fs::path::string_type(_STR("foo/")));
        CHECK(fs::path(_STR("foo/")).remove_filename() == fs::path::string_type(_STR("foo/")));
        CHECK(fs::path(_STR("/foo")).remove_filename() == fs::path::string_type(_STR("/")));
        CHECK(fs::path(_STR("/")).remove_filename() == fs::path::string_type(_STR("/")));
    }

////////////////////////////////////////////////////////////////////////////////
// Modifiers #4
////////////////////////////////////////////////////////////////////////////////
    {
        CHECK(fs::path(_STR("/foo")).replace_filename(_STR("bar")) == fs::path::string_type(_STR("/bar")));
        CHECK(fs::path(_STR("/")).replace_filename(_STR("bar")) == fs::path::string_type(_STR("/bar"))); // FIXME for GHC version
    }

////////////////////////////////////////////////////////////////////////////////
// Modifiers #5
////////////////////////////////////////////////////////////////////////////////
    {
        fs::path p = _STR("/foo/bar.jpeg");

        CHECK(p == fs::path::string_type(_STR("/foo/bar.jpeg")));

        p.replace_extension(_STR(".jpg"));

        CHECK(p == fs::path::string_type(_STR("/foo/bar.jpg")));
    }

////////////////////////////////////////////////////////////////////////////////
// Modifiers #6
////////////////////////////////////////////////////////////////////////////////
    {
        fs::path p1 = _STR("/foo/bar");
        fs::path p2 = _STR("/foo/baz");

        CHECK(p1 == fs::path::string_type(_STR("/foo/bar")));
        CHECK(p2 == fs::path::string_type(_STR("/foo/baz")));

        p1.swap(p2);

        CHECK(p1 == fs::path::string_type(_STR("/foo/baz")));
        CHECK(p2 == fs::path::string_type(_STR("/foo/bar")));
    }

////////////////////////////////////////////////////////////////////////////////
// Compare
////////////////////////////////////////////////////////////////////////////////
    {
        fs::path p1 = _STR("/a/b/"); // as if "a/b/." for lexicographical iteration
        fs::path p2 = _STR("/a/b/#");
        fs::path p3 = _STR("/a/b/_");

        CHECK(p1.compare(p1) == 0);
        CHECK(p1.compare(p2) < 0); // FIXME for GHC version
        CHECK(p1.compare(p3) < 0);
    }

////////////////////////////////////////////////////////////////////////////////
// Generation
////////////////////////////////////////////////////////////////////////////////
    {
       CHECK(fs::path(_STR("foo/./bar/..")).lexically_normal() == _STR("foo/"));
       CHECK(fs::path(_STR("foo/.///bar/../")).lexically_normal() == _STR("foo/"));
       CHECK(fs::path(_STR("/a/d")).lexically_relative(_STR("/a/b/c")) == _STR("../../d"));
       CHECK(fs::path(_STR("/a/b/c")).lexically_relative(_STR("/a/d")) == _STR("../b/c"));
       CHECK(fs::path(_STR("a/b/c")).lexically_relative(_STR("a")) == _STR("b/c"));
       CHECK(fs::path(_STR("a/b/c")).lexically_relative(_STR("a/b/c/x/y")) == _STR("../.."));
       CHECK(fs::path(_STR("a/b/c")).lexically_relative(_STR("a/b/c")) == _STR("."));
       CHECK(fs::path(_STR("a/b")).lexically_relative(_STR("c/d")) == _STR("../../a/b"));
    }

////////////////////////////////////////////////////////////////////////////////
// Decomposition
////////////////////////////////////////////////////////////////////////////////
    {
#if defined(_MSC_VER)
        CHECK(fs::path(_STR("//server/path/to/file")).root_name() == fs::path(_STR("//server")));
        CHECK(fs::path(_STR("//server/path/to/file")).root_path() == fs::path(_STR("//server/")));
        CHECK(fs::path(_STR("//server/path/to/file")).relative_path() == fs::path(_STR("path/to/file")));
        //CHECK(fs::path(_STR("/")).parent_path() == fs::path(_STR(""))); // FIXME for GHC version
#else
#   if !defined(PFS_NO_STD_FILESYSTEM)
        CHECK(fs::path(_STR("//server/path/to/file")).root_name() == fs::path());
        CHECK(fs::path(_STR("//server/path/to/file")).root_path() == fs::path(_STR("/")));
        CHECK(fs::path(_STR("//server/path/to/file")).relative_path() == fs::path(_STR("server/path/to/file")));
#   else
        CHECK(fs::path(_STR("//server/path/to/file")).root_name() == fs::path(_STR("//server")));
        CHECK(fs::path(_STR("//server/path/to/file")).root_path() == fs::path(_STR("//server/")));
        CHECK(fs::path(_STR("//server/path/to/file")).relative_path() == fs::path(_STR("path/to/file")));
#   endif

        CHECK(fs::path(_STR("/")).parent_path() == fs::path(_STR("/")));
#endif

        CHECK(fs::path(_STR("//server/path/to/file")).root_directory() == fs::path(_STR("/")));
        CHECK(fs::path(_STR("//server/path/to/file")).parent_path() == fs::path(_STR("//server/path/to")));
        CHECK(fs::path(_STR("/path/to/.")).parent_path() == fs::path(_STR("/path/to")));
        CHECK(fs::path(_STR("/foo/bar.txt")).filename() == fs::path(_STR("bar.txt")));
        CHECK(fs::path(_STR("/foo/.bar")).filename() == fs::path(_STR(".bar")));

        CHECK(fs::path(_STR("/foo/bar/")).filename() == fs::path(_STR("")));
        CHECK(fs::path(_STR("/foo/.")).filename() == fs::path(_STR(".")));
        CHECK(fs::path(_STR("/foo/..")).filename() == fs::path(_STR("..")));
        CHECK(fs::path(_STR(".")).filename() == fs::path(_STR(".")));
        CHECK(fs::path(_STR("..")).filename() == fs::path(_STR("..")));
        CHECK(fs::path(_STR("/")).filename() == fs::path(_STR("")));

        CHECK(fs::path(_STR("/foo/bar.txt")).stem() == fs::path::string_type(_STR("bar")));
        CHECK(fs::path(_STR(".hidden")).stem() == fs::path::string_type(_STR(".hidden")));
        CHECK(fs::path(_STR("/foo/.hidden")).stem() == fs::path::string_type(_STR(".hidden")));

        CHECK(fs::path(_STR("/foo/bar.txt")).extension() == fs::path::string_type(_STR(".txt")));
        CHECK(fs::path(_STR("/foo/bar.")).extension() == fs::path::string_type(_STR(".")));
        CHECK(fs::path(_STR("/foo/bar")).extension() == fs::path::string_type(_STR("")));
        CHECK(fs::path(_STR("/foo/bar.txt/bar.cc")).extension() == fs::path::string_type(_STR(".cc")));
        CHECK(fs::path(_STR("/foo/bar.txt/bar.")).extension() == fs::path::string_type(_STR(".")));
        CHECK(fs::path(_STR("/foo/bar.txt/bar")).extension() == fs::path::string_type(_STR("")));
        CHECK(fs::path(_STR("/foo/.")).extension() == fs::path::string_type(_STR("")));

#   if !defined(PFS_NO_STD_FILESYSTEM)
        CHECK(fs::path(_STR("/foo/..")).extension() == fs::path::string_type(_STR("")));
#   else
        CHECK(fs::path(_STR("/foo/..")).extension() == fs::path::string_type(_STR("."))); // FIXME for GHC version
#   endif

        CHECK(fs::path(_STR(".hidden")).extension() == fs::path::string_type(_STR("")));
        CHECK(fs::path(_STR("/foo/.hidden")).extension() == fs::path::string_type(_STR("")));
        CHECK(fs::path(_STR("/foo/..bar")).extension() == fs::path::string_type(_STR(".bar")));

#if defined(_WIN32) || defined(_WIN64)
        CHECK(fs::path(_STR("c:\\path\\to\\file")).root_name() == fs::path(_STR("c:")));
        CHECK(fs::path(_STR("c:\\path\\to\\file")).root_directory() == fs::path(_STR("\\")));
        CHECK(fs::path(_STR("c:\\path\\to\\file")).root_path() == fs::path(_STR("c:\\")));
        CHECK(fs::path(_STR("c:\\path\\to\\file")).relative_path() == fs::path(_STR("path\\to\\file")));

        CHECK(fs::path(_STR("c:\\path\\to\\file")).parent_path() == fs::path(_STR("c:\\path\\to")));
        CHECK(fs::path(_STR("c:\\path\\to\\.")).parent_path() == fs::path(_STR("c:\\path\\to")));
        CHECK(fs::path(_STR("\\")).parent_path() == fs::path(_STR("\\")));
#endif
    }

////////////////////////////////////////////////////////////////////////////////
// Queries
////////////////////////////////////////////////////////////////////////////////
    {
        CHECK(!fs::path("//server/path/to/file").empty());
        CHECK(fs::path("").empty());
        CHECK(fs::path().empty());

#if defined(_WIN32) || defined(_WIN64)
        CHECK(fs::path("//server/path/to/file").has_root_name());
#else
#   if !defined(PFS_NO_STD_FILESYSTEM)
        CHECK_FALSE(fs::path("//server/path/to/file").has_root_name());
#   else
        CHECK(fs::path("//server/path/to/file").has_root_name()); // FIXME for GHC version
#   endif
#endif
        CHECK(fs::path("//server/path/to/file").has_root_path());
        CHECK(fs::path("//server/path/to/file").has_root_directory());
        CHECK(fs::path("//server/path/to/file").has_relative_path());

        CHECK(fs::path("//server/path/to/file").has_parent_path());
        CHECK(fs::path("/path/to/.").has_parent_path());
        CHECK(fs::path("/").has_parent_path());

        CHECK(fs::path("/foo/bar.txt").has_filename());
        CHECK(fs::path("/foo/.bar").has_filename());

        CHECK_FALSE(fs::path("/foo/bar/").has_filename());
        CHECK(fs::path("/foo/.").has_filename());
        CHECK(fs::path("/foo/..").has_filename());
        CHECK(fs::path(".").has_filename());
        CHECK(fs::path("..").has_filename());
        CHECK_FALSE(fs::path("/").has_filename());

        CHECK(fs::path("/foo/bar.txt").has_stem());
        CHECK(fs::path(".hidden").has_stem());
        CHECK(fs::path("/foo/.hidden").has_stem());

        CHECK(fs::path("/foo/bar.txt").has_extension());
        CHECK(fs::path("/foo/bar.").has_extension());
        CHECK(!fs::path("/foo/bar").has_extension());
        CHECK(fs::path("/foo/bar.txt/bar.cc").has_extension());
        CHECK(fs::path("/foo/bar.txt/bar.").has_extension());
        CHECK(!fs::path("/foo/bar.txt/bar").has_extension());
        CHECK(!fs::path("/foo/.").has_extension());

#   if !defined(PFS_NO_STD_FILESYSTEM)
        CHECK_FALSE(fs::path("/foo/..").has_extension());
#   else
        CHECK(fs::path("/foo/..").has_extension()); // FIXME for GHC version
#   endif

        CHECK(!fs::path(".hidden").has_extension());

        CHECK_FALSE(fs::path("/foo/.hidden").has_extension());
        CHECK(fs::path("/foo/..bar").has_extension());

        CHECK(fs::path("path/to").is_relative());

#if defined(_WIN32) || defined(_WIN64)
        // The path "/" is absolute on a POSIX OS, but is relative on Windows.
        CHECK(fs::path("/").is_relative());
#else
        // The path "/" is absolute on a POSIX OS, but is relative on Windows.
        CHECK(fs::path("/").is_absolute());
#endif
    }

////////////////////////////////////////////////////////////////////////////////
// Iterators
////////////////////////////////////////////////////////////////////////////////
    {
#if defined(_WIN32) || defined(_WIN64)
        fs::path p = "C:\\users\\abcdef\\AppData\\Local\\Temp\\";
        fs::path::iterator it = p.begin();

        CHECK(*it++ == "C:");
        CHECK(*it++ == "/");
        CHECK(*it++ == "users");
        CHECK(*it++ == "abcdef");
        CHECK(*it++ == "AppData");
        CHECK(*it++ == "Local");
        CHECK(*it++ == "Temp");
        CHECK(*it++ == "");
        CHECK(it == p.end());
#else
        fs::path p = "/home/user/abcdef/app_data/Local/Temp/";
        fs::path::iterator it = p.begin();

        CHECK(*it++ == "/");
        CHECK(*it++ == "home");
        CHECK(*it++ == "user");
        CHECK(*it++ == "abcdef");
        CHECK(*it++ == "app_data");
        CHECK(*it++ == "Local");
        CHECK(*it++ == "Temp");
        //CHECK(*it++ == ".");
        CHECK(*it++ == "");    // Valid for GHC version
        CHECK(it == p.end());
#endif
    }

////////////////////////////////////////////////////////////////////////////////
// Non-member functions
////////////////////////////////////////////////////////////////////////////////
    {
        using std::swap;
        fs::path p1("/path/to1");
        fs::path p2("/path/to2");

        CHECK(p1 == fs::path("/path/to1"));
        CHECK(p2 == fs::path("/path/to2"));

        swap(p1, p2);

        CHECK(p1 == fs::path("/path/to2"));
        CHECK(p2 == fs::path("/path/to1"));
    }

    {
        fs::path p1("/path/to");
        fs::path p2("/path/to");
        fs::path p3("/path/to/a");

        CHECK(p1 == p2);
        CHECK(p1 <= p2);
        CHECK(p1 >= p2);

        CHECK(p1 < p3);
        CHECK(p1 <= p3);

        CHECK(p3 > p1);
        CHECK(p3 >= p1);
    }
}

////////////////////////////////////////////////////////////////////////////////
// directory_entry
////////////////////////////////////////////////////////////////////////////////
TEST_CASE("Filesystem directory_entry") {
#if defined(PFS_NO_STD_FILESYSTEM)
    namespace fs = pfs::filesystem;
#else
    namespace fs = std::filesystem;
#endif

    // Constructors
    {
//         fs::directory_entry p1("/usr/lib"); // portable format
//         fs::directory_entry p2("C:\\users\\abcdef\\AppData\\Local\\Temp\\"); // native format
// #if defined(_WIN32) || defined(_WIN64)
//         fs::directory_entry p3(L"D:/猫.txt"); // wide string
// #else
//         fs::directory_entry p3("D:/猫.txt");
// #endif
//
//         //        std::cout << "p1 = " << p1 << '\n'
//         //                  << "p2 = " << p2 << '\n'
//         //                  << "p3 = " << p3 << '\n';
//
//         CHECK(p1 == fs::path::string_type("/usr/lib/sendmail.cf"));
//         CHECK(p2 == fs::path::string_type("C:\\users\\abcdef\\AppData\\Local\\Temp\\"));
// #if defined(_WIN32) || defined(_WIN64)
//         CHECK(p3 == fs::path::string_type(L"D:/猫.txt"));
// #else
//         CHECK(p3 == fs::path::string_type("D:/猫.txt"));
// #endif
    }
}
