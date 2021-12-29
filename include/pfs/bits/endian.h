/*//////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2017.07.03 Initial version.
//      2021.10.14 Included from old `pfs` library.
//////////////////////////////////////////////////////////////////////////////*/
#pragma once

/*
 * https://sourceforge.net/p/predef/wiki/Endianness/
 */

#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__)
    /* That's all need to know about endianess */
#else
#   ifndef __ORDER_LITTLE_ENDIAN__
#       define __ORDER_LITTLE_ENDIAN__ 1234
#   endif

#   ifndef __ORDER_BIG_ENDIAN__
#       define __ORDER_BIG_ENDIAN__ 4321
#   endif

#   ifndef __ORDER_PDP_ENDIAN__
#       define __ORDER_PDP_ENDIAN__ 3412
#   endif

#   if defined(_MSC_VER)                                                       \
        || defined(MSDOS)     /* MS-DOS is an operation system */              \
        || defined(__MSDOS__) /* for x86-based personal computers */           \
        || defined(_MSDOS)
#       define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__
#   endif

#   if defined(__elbrus_4c__) /* Elbrus 4C - little endian */
#       define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__
#   endif

/*
 * X86 family, known variants: 32- and 64-bit
 *
 * X86 is little-endian.
 */
#   if defined(__i386) || defined(__i386__)                                    \
        || defined(__x86_64) || defined(__x86_64__)                            \
        || defined(__amd64)                                                    \
        || defined(_M_IX86) || defined(_M_X64)
#       define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__
#   endif

/*
 * ARM family, known revisions: V5, V6, and V7
 *
 * ARM is bi-endian, detect using __ARMEL__ or __ARMEB__, falling back to
 * auto-detection implemented below.
 */
#   if defined(__arm__) || defined(__TARGET_ARCH_ARM) || defined(_M_ARM)
#       if defined(__ARMEL__)
#           define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__
#       elif defined(__ARMEB__)
#           define __BYTE_ORDER__ __ORDER_BIG_ENDIAN__
#       endif
#   endif

#   if defined(__ARMEL__) || defined(__THUMBEL__) || defined(__AARCH64EL__)    \
        || defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__)
#           define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__
#   endif

#   if defined(__ARMEB__) || defined(__THUMBEB__) || defined(__AARCH64EB__)    \
        || defined(_MIPSEB) || defined(__MIPSEB) || defined(__MIPSEB__)
#           define __BYTE_ORDER__ __ORDER_BIG_ENDIAN__
#   endif
#endif
