#pragma once

#if defined(WIN64) || defined(_WIN64)
#   define PFS_OS_WIN64 1
#elif defined(WIN32) || defined(_WIN32)
#   define PFS_OS_WIN32 1
#elif defined(__linux__) || defined(__linux)
#   define PFS_OS_LINUX 1
#else
#   error "Unsupported Platform"
#endif

#if defined(PFS_OS_WIN32) || defined(PFS_OS_WIN64)
#   define PFS_OS_WIN 1
#endif

#if (defined(__WORDSIZE) && __WORDSIZE == 64)                                  \
    || (defined(UINTPTR_MAX) && UINTPTR_MAX == 0xFFFFFFFFFFFFFFFF)             \
    || defined(_WIN64)                                                         \
    || (defined(__GNUC__) && (__x86_64__ || __ppc64__))                        \
    || defined(__LP64__)                                                       \
    || defined(_LP64)
#   define PFS_OS_64BITS
#   define PFS_OS_BITS    64
#else
#   define PFS_OS_32BITS
#   define PFS_OS_BITS    32
#endif
