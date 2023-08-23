/*//////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2023 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2017.07.03 Initial version (as part of https://github.com/semenovf/pfs).
//      2021.10.13 Included in common-lib.
//      2023.08.23 Changed preferred prefix for macros: PFS__.
//////////////////////////////////////////////////////////////////////////////*/
#pragma once

#if defined(_MSC_VER)
#   define PFS_COMPILER_DEFINED 1 // DEPRECATED
#   define PFS_COMPILER_MSVC 1 // DEPRECATED
#   define PFS_COMPILER_MSC 1 // DEPRECATED
#   define PFS__COMPILER_DEFINED 1
#   define PFS__COMPILER_MSVC 1

/*
 * see http://stackoverflow.com/questions/70013/how-to-detect-if-im-compiling-code-with-visual-studio-2008
 * MSVC++ 12.0 _MSC_VER == 1800 (Visual Studio 2013)
 * MSVC++ 11.0 _MSC_VER == 1700 (Visual Studio 2012)
 * MSVC++ 10.0 _MSC_VER == 1600 (Visual Studio 2010)
 * MSVC++ 9.0  _MSC_VER == 1500 (Visual Studio 2008)
 * MSVC++ 8.0  _MSC_VER == 1400 (Visual Studio 2005)
 * MSVC++ 7.1  _MSC_VER == 1310 (Visual Studio 2003)
 * MSVC++ 7.0  _MSC_VER == 1300
 * MSVC++ 6.0  _MSC_VER == 1200
 * MSVC++ 5.0  _MSC_VER == 1100
 */
#   define PFS_COMPILER_MSVC_VERSION _MSC_VER // DEPRECATED

#   define PFS__COMPILER_MSVC_VERSION _MSC_VER /* In Visual Studio 2010, _MSC_VER is defined as 1600 */
#elif defined(__clang__)
// echo | clang -dM -E -
// echo | clang++ -dM -E -x c++ -

#   define PFS_COMPILER_DEFINED 1 // DEPRECATED
#   define PFS_COMPILER_CLANG 1 // DEPRECATED
#   define PFS__COMPILER_DEFINED 1
#   define PFS__COMPILER_CLANG 1

#   define PFS__COMPILER_CLANG_VERSION \
        (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)
#elif defined(__GNUC__)
// echo | gcc -dM -E -
// echo | clang -dM -E -

#   define PFS_COMPILER_DEFINED 1 // DEPRECATED
#   define PFS_COMPILER_GNUC 1 // DEPRECATED
#   define PFS_COMPILER_GCC 1 // DEPRECATED
#   define PFS__COMPILER_DEFINED 1
#   define PFS__COMPILER_GCC 1

/*
 * [Common Predefined Macros](https://gcc.gnu.org/onlinedocs/gcc-4.0.1/cpp/Common-Predefined-Macros.html)
 */
#   define PFS_COMPILER_GCC_VERSION \
        (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__) // DEPRECATED
#   define PFS_COMPILER_GNUC_VERSION PFS_COMPILER_GCC_VERSION // DEPRECATED

#   define PFS__COMPILER_GCC_VERSION \
        (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#   define PFS__COMPILER_GNUC_VERSION PFS__COMPILER_GCC_VERSION

// #   if defined(__clang__)
/* Clang also masquerades as GCC 4.2.1 */
// #       define PFS__COMPILER_CLANG 1
// #   endif
#endif
