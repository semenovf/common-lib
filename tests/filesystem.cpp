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
#include "pfs/filesystem.hpp"

TEST_CASE("Filesystem path") {
    namespace fs = pfs::filesystem;

////////////////////////////////////////////////////////////////////////////////
// Constructors                                                               //
////////////////////////////////////////////////////////////////////////////////
    {
        fs::path p_from_cstr("/usr/lib/sendmail.cf");
        fs::path p_from_pfs_string(std::string("/usr/lib/sendmail.cf"));

        fs::path p1 = "/usr/lib/sendmail.cf"; // portable format
        fs::path p2 = "C:\\users\\abcdef\\AppData\\Local\\Temp\\"; // native format
#if defined(_WIN32) || defined(_WIN64)
        fs::path p3 = L"D:/猫.txt";           // wide string
#else
        fs::path p3 = "D:/猫.txt";
#endif

        CHECK(p1 == fs::path::string_type("/usr/lib/sendmail.cf"));
        CHECK(p2 == fs::path::string_type("C:\\users\\abcdef\\AppData\\Local\\Temp\\"));
#if defined(_WIN32) || defined(_WIN64)
        CHECK(p3 == fs::path::string_type(L"D:/猫.txt"));
#else
        CHECK(p3 == fs::path::string_type("D:/猫.txt"));
#endif
    }

////////////////////////////////////////////////////////////////////////////////
// Assign operators
////////////////////////////////////////////////////////////////////////////////
    {
        fs::path p = "C:/users/abcdef/AppData/Local";
        p = p / "Temp"; // move assignment

        CHECK(p == fs::path::string_type("C:/users/abcdef/AppData/Local/Temp"));

        wchar_t const * wstr = L"D:/猫.txt";
        p = wstr; // assignment from a source

        CHECK(p == fs::path::string_type("D:/猫.txt"));
    }

////////////////////////////////////////////////////////////////////////////////
// Concatenation #1
////////////////////////////////////////////////////////////////////////////////
    {
        // where "//host" is a root-name
        CHECK((fs::path("//host") /= "foo") == fs::path::string_type("//host/foo")); // appends with separator // FIXME for GHC version
        CHECK((fs::path("//host/") /= "foo") == fs::path::string_type("//host/foo")); // appends without separator // FIXME for GHC version

        // Non-member function
        CHECK(fs::path("//host") / "foo"  == fs::path::string_type("//host/foo")); // appends with separator
        CHECK(fs::path("//host/") / "foo" == fs::path::string_type("//host/foo")); // appends without separator

#if defined(_WIN32) || defined(_WIN64)
        CHECK((fs::path("foo") /= "C:/bar") == fs::path::string_type("C:/bar"));     // the result is "C:/bar" (replaces)
        CHECK((fs::path("foo") /= "C:") == fs::path::string_type("C:"));             // the result is "C:"     (replaces)
        CHECK((fs::path("C:") /= "") == fs::path::string_type("C:"));                // the result is "C:"     (appends, without separator)
        CHECK((fs::path("C:foo") /= "/bar") == fs::path::string_type("C:/bar"));     // yields "C:/bar"        (removes relative path, then appends)
        CHECK((fs::path("C:foo") /= "C:bar") == fs::path::string_type("C:foo/bar")); // yields "C:foo/bar"     (appends, omitting p's root-name)

        // Non-member function
        CHECK(fs::path("foo") / "C:/bar" == fs::path::string_type("C:/bar"));     // the result is "C:/bar" (replaces)
        CHECK(fs::path("foo") / "C:" == fs::path::string_type("C:"));             // the result is "C:"     (replaces)
        CHECK(fs::path("C:") / "" == fs::path::string_type("C:"));                // the result is "C:"     (appends, without separator)
        CHECK(fs::path("C:foo") / "/bar" == fs::path::string_type("C:/bar"));     // yields "C:/bar"        (removes relative path, then appends)
        CHECK(fs::path("C:foo") / "C:bar" == fs::path::string_type("C:foo/bar")); // yields "C:foo/bar"     (appends, omitting p's root-name)
#else
        CHECK((fs::path("foo") /= "") == fs::path::string_type("foo")); // the result is "foo/" (appends) // FIXME for GHC version
        CHECK((fs::path("foo") /= "/bar") == fs::path::string_type("foo/bar")); // the result is "/bar" (replaces) // FIXME for GHC version

        // Non-member function
        CHECK(fs::path("foo") / "" == fs::path::string_type("foo")); // the result is "foo/" (appends) // FIXME for GHC version
        CHECK(fs::path("foo") / "/bar" == fs::path::string_type("foo/bar")); // the result is "/bar" (replaces) // FIXME for GHC version
#endif
    }

////////////////////////////////////////////////////////////////////////////////
// Concatenation #2
////////////////////////////////////////////////////////////////////////////////
    {
        fs::path p; // empty path
        p += "var"; // does not insert a separator

        CHECK(p == fs::path::string_type("var"));

        p += "lib"; // does not insert a separator

        CHECK(p == fs::path::string_type("varlib"));
    }

////////////////////////////////////////////////////////////////////////////////
// Modifiers #1
////////////////////////////////////////////////////////////////////////////////
    {
        fs::path p("/path/to/file");

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
        CHECK(fs::path("foo/bar").remove_filename() == fs::path::string_type("foo/"));
        CHECK(fs::path("foo/").remove_filename() == fs::path::string_type("foo/"));
        CHECK(fs::path("/foo").remove_filename() == fs::path::string_type("/"));
        CHECK(fs::path("/").remove_filename() == fs::path::string_type("/"));
    }

////////////////////////////////////////////////////////////////////////////////
// Modifiers #4
////////////////////////////////////////////////////////////////////////////////
    {
        CHECK(fs::path("/foo").replace_filename("bar") == fs::path::string_type("/bar"));
        CHECK(fs::path("/").replace_filename("bar") == fs::path::string_type("bar")); // FIXME for GHC version
    }

////////////////////////////////////////////////////////////////////////////////
// Modifiers #5
////////////////////////////////////////////////////////////////////////////////
    {
        fs::path p = "/foo/bar.jpeg";

        CHECK(p == fs::path::string_type("/foo/bar.jpeg"));

        p.replace_extension(".jpg");

        CHECK(p == fs::path::string_type("/foo/bar.jpg"));
    }

////////////////////////////////////////////////////////////////////////////////
// Modifiers #6
////////////////////////////////////////////////////////////////////////////////
    {
        fs::path p1 = "/foo/bar";
        fs::path p2 = "/foo/baz";

        CHECK(p1 == fs::path::string_type("/foo/bar"));
        CHECK(p2 == fs::path::string_type("/foo/baz"));

        p1.swap(p2);

        CHECK(p1 == fs::path::string_type("/foo/baz"));
        CHECK(p2 == fs::path::string_type("/foo/bar"));
    }

////////////////////////////////////////////////////////////////////////////////
// Compare
////////////////////////////////////////////////////////////////////////////////
    {
        fs::path p1 = "/a/b/"; // as if "a/b/." for lexicographical iteration
        fs::path p2 = "/a/b/#";
        fs::path p3 = "/a/b/_";

        CHECK(p1.compare(p1) == 0);
        CHECK(p1.compare(p2) > 0); // FIXME for GHC version
        CHECK(p1.compare(p3) < 0);
    }

////////////////////////////////////////////////////////////////////////////////
// Generation
////////////////////////////////////////////////////////////////////////////////
    {
       CHECK(fs::path("foo/./bar/..").lexically_normal() == "foo/");
       CHECK(fs::path("foo/.///bar/../").lexically_normal() == "foo/");
       CHECK(fs::path("/a/d").lexically_relative("/a/b/c") == "../../d");
       CHECK(fs::path("/a/b/c").lexically_relative("/a/d") == "../b/c");
       CHECK(fs::path("a/b/c").lexically_relative("a") == "b/c");
       CHECK(fs::path("a/b/c").lexically_relative("a/b/c/x/y") == "../..");
       CHECK(fs::path("a/b/c").lexically_relative("a/b/c") == ".");
       CHECK(fs::path("a/b").lexically_relative("c/d") == "../../a/b");
    }

////////////////////////////////////////////////////////////////////////////////
// Decomposition
////////////////////////////////////////////////////////////////////////////////
    {
        CHECK(fs::path("//server/path/to/file").root_name() == fs::path("//server"));
        CHECK(fs::path("//server/path/to/file").root_directory() == fs::path("/"));
        CHECK(fs::path("//server/path/to/file").root_path() == fs::path("//server/"));
        CHECK(fs::path("//server/path/to/file").relative_path() == fs::path("path/to/file"));

        CHECK(fs::path("//server/path/to/file").parent_path() == fs::path("//server/path/to"));
        CHECK(fs::path("/path/to/.").parent_path() == fs::path("/path/to"));
        CHECK(fs::path("/").parent_path() == fs::path("")); // FIXME for GHC version

        CHECK(fs::path("/foo/bar.txt").filename() == fs::path("bar.txt"));
        CHECK(fs::path("/foo/.bar").filename() == fs::path(".bar"));
        CHECK(fs::path("/foo/bar/").filename() == fs::path(".")); // FIXME for GHC version
        CHECK(fs::path("/foo/.").filename() == fs::path("."));
        CHECK(fs::path("/foo/..").filename() == fs::path(".."));
        CHECK(fs::path(".").filename() == fs::path("."));
        CHECK(fs::path("..").filename() == fs::path(".."));
        CHECK(fs::path("/").filename() == fs::path("/")); // FIXME for GHC version

        CHECK(fs::path("/foo/bar.txt").stem() == fs::path::string_type("bar"));
        CHECK(fs::path(".hidden").stem() == fs::path::string_type(".hidden"));
        CHECK(fs::path("/foo/.hidden").stem() == fs::path::string_type(".hidden")); // FIXME (for std::experimental::filesystem::path::stem() returns "")

        CHECK(fs::path("/foo/bar.txt").extension() == fs::path::string_type(".txt"));
        CHECK(fs::path("/foo/bar.").extension() == fs::path::string_type("."));
        CHECK(fs::path("/foo/bar").extension() == fs::path::string_type(""));
        CHECK(fs::path("/foo/bar.txt/bar.cc").extension() == fs::path::string_type(".cc"));
        CHECK(fs::path("/foo/bar.txt/bar.").extension() == fs::path::string_type("."));
        CHECK(fs::path("/foo/bar.txt/bar").extension() == fs::path::string_type(""));
        CHECK(fs::path("/foo/.").extension() == fs::path::string_type(""));
        CHECK(fs::path("/foo/..").extension() == fs::path::string_type("")); // FIXME for GHC version
        CHECK(fs::path(".hidden").extension() == fs::path::string_type(""));
        CHECK(fs::path("/foo/.hidden").extension() == fs::path::string_type("")); // FIXME (for std::experimental::filesystem::path::extension() returns ".hidden")
        CHECK(fs::path("/foo/..bar").extension() == fs::path::string_type(".bar"));

#if defined(_WIN32) || defined(_WIN64)
        CHECK(fs::path("c:\\path\\to\\file").root_name() == fs::path("c:"));
        CHECK(fs::path("c:\\path\\to\\file").root_directory() == fs::path("\\"));
        CHECK(fs::path("c:\\path\\to\\file").root_path() == fs::path("c:\\"));
        CHECK(fs::path("c:\\path\\to\\file").relative_path() == fs::path("path\\to\\file"));

        CHECK(fs::path("c:\\path\\to\\file").parent_path() == fs::path("c:\\path\\to"));
        CHECK(fs::path("c:\\path\\to\\.").parent_path() == fs::path("c:\\path\\to"));
        CHECK(fs::path("\\").parent_path() == fs::path(""));
#endif
    }

////////////////////////////////////////////////////////////////////////////////
// Queries
////////////////////////////////////////////////////////////////////////////////
    {
        CHECK(!fs::path("//server/path/to/file").empty());
        CHECK(fs::path("").empty());
        CHECK(fs::path().empty());

        CHECK(fs::path("//server/path/to/file").has_root_path());
        CHECK(fs::path("//server/path/to/file").has_root_name());
        CHECK(fs::path("//server/path/to/file").has_root_directory());
        CHECK(fs::path("//server/path/to/file").has_relative_path());

        CHECK(fs::path("//server/path/to/file").has_parent_path());
        CHECK(fs::path("/path/to/.").has_parent_path());
        CHECK(!fs::path("/").has_parent_path()); // FIXME for GHC version

        CHECK(fs::path("/foo/bar.txt").has_filename());
        CHECK(fs::path("/foo/.bar").has_filename());
        CHECK(fs::path("/foo/bar/").has_filename()); // FIXME for GHC version
        CHECK(fs::path("/foo/.").has_filename());
        CHECK(fs::path("/foo/..").has_filename());
        CHECK(fs::path(".").has_filename());
        CHECK(fs::path("..").has_filename());
        CHECK(fs::path("/").has_filename()); // FIXME for GHC version

        CHECK(fs::path("/foo/bar.txt").has_stem());
        CHECK(fs::path(".hidden").has_stem());
        CHECK(fs::path("/foo/.hidden").has_stem()); // FIXME (for std::experimental::filesystem::path::stem() returns "")

        CHECK(fs::path("/foo/bar.txt").has_extension());
        CHECK(fs::path("/foo/bar.").has_extension());
        CHECK(!fs::path("/foo/bar").has_extension());
        CHECK(fs::path("/foo/bar.txt/bar.cc").has_extension());
        CHECK(fs::path("/foo/bar.txt/bar.").has_extension());
        CHECK(!fs::path("/foo/bar.txt/bar").has_extension());
        CHECK(!fs::path("/foo/.").has_extension());
        CHECK(!fs::path("/foo/..").has_extension()); // FIXME for GHC version
        CHECK(!fs::path(".hidden").has_extension());
        CHECK(!fs::path("/foo/.hidden").has_extension()); // FIXME (for std::experimental::filesystem::path::stem() returns ".hidden")
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
        CHECK(*it++ == ".");
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
        CHECK(*it++ == "."); // FIXME for GHC version
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
    namespace fs = pfs::filesystem;

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
