////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2024 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2024.10.05 Initial version.
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
// [/Zc:__cplusplus (Enable updated __cplusplus macro)](https://learn.microsoft.com/en-us/cpp/build/reference/zc-cplusplus?view=msvc-170)
// 
// The /Zc:__cplusplus compiler option enables the __cplusplus preprocessor 
// macro to report an updated value for recent C++ language standards support. 
// By default, Visual Studio always returns the value 199711L for the 
// __cplusplus preprocessor macro.
////////////////////////////////////////////////////////////////////////////////

// NOTE: There is no /std:c++11 option.
#define MESSAGE_TEXT "With set __cplusplus with /std:c++14 as compiler options"

TEST_CASE("test") {
    std::cout << "__cplusplus=" << __cplusplus << std::endl;

#if __cplusplus >= 201103L
    CHECK_MESSAGE(true, MESSAGE_TEXT);
#else
    CHECK_MESSAGE(false, MESSAGE_TEXT);
#endif
}