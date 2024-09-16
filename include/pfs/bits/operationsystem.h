/*//////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2018.05.11 Initial version (as part of https://github.com/semenovf/pfs).
//      2021.09.23 Included in common-lib.
//      2023.07.08 Added Android ABI macros.
//////////////////////////////////////////////////////////////////////////////*/
#pragma once

#if defined(WIN64) || defined(_WIN64)
#   define PFS_OS_WIN64 1 // DEPRECATED
#   define PFS__OS_WIN64 1
#elif defined(WIN32) || defined(_WIN32)
#   define PFS_OS_WIN32 1 // DEPRECATED
#   define PFS__OS_WIN32 1
#elif defined(ANDROID) || defined(__ANDROID__) // Check before __linux__
#   define PFS_OS_ANDROID 1 // DEPRECATED
#   define PFS__OS_ANDROID 1

// clang -dM -E - < /dev/null
#   if defined(__i386) || defined(__i386__) || defined(__i686) || defined(__i686__)
#       define PFS__OS_ANDROID_ABI_X86 1
#   elif defined(__x86_64) || defined(__x86_64__)
#       define PFS__OS_ANDROID_ABI_X86_64 1
#   elif defined(__aarch64_1)
#       define PFS__OS_ANDROID_ABI_AARCH64 1
#       define PFS__OS_ANDROID_ABI_ARM8 1
#   elif defined(__arm) || defined(__arm__)
#       define PFS__OS_ANDROID_ABI_ARM 1
#       define PFS__OS_ANDROID_ABI_ARM7 1
#   else
#       define PFS__OS_ANDROID_ABI_OTHER 1
#   endif
#elif (defined(__linux__) || defined(__linux))
#   define PFS_OS_LINUX 1 // DEPRECATED
#   define PFS__OS_LINUX 1
#else
#   error "Unsupported Platform"
#endif

#if defined(PFS__OS_WIN32) || defined(PFS__OS_WIN64)
#   define PFS_OS_WIN 1 // DEPRECATED
#   define PFS__OS_WIN 1
#endif

#if (defined(__WORDSIZE) && __WORDSIZE == 64)                                  \
    || (defined(UINTPTR_MAX) && UINTPTR_MAX == 0xFFFFFFFFFFFFFFFF)             \
    || defined(_WIN64)                                                         \
    || (defined(__GNUC__) && (__x86_64__ || __ppc64__))                        \
    || defined(__LP64__)                                                       \
    || defined(_LP64)
#   define PFS_OS_64BITS // DEPRECATED
#   define PFS__OS_64BITS 1
#   define PFS_OS_BITS    64 // DEPRECATED
#   define PFS__OS_BITS   64
#else
#   define PFS_OS_32BITS // DEPRECATED
#   define PFS__OS_32BITS 1
#   define PFS_OS_BITS    32 // DEPRECATED
#   define PFS__OS_BITS   32
#endif
