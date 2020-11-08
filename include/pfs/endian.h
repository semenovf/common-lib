////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of [pfs-common](https://github.com/semenovf/pfs-common) library.
//
// Changelog:
//      2020.11.08 Initial version
//
////////////////////////////////////////////////////////////////////////////////
#pragma once

#undef LITTLE_ENDIAN_PLATFORM
#undef BIG_ENDIAN_PLATFORM

#if defined(__BYTE_ORDER)
#   if __BYTE_ORDER == __LITTLE_ENDIAN
#       define LITTLE_ENDIAN_PLATFORM 1
#   elif __BYTE_ORDER == __BIG_ENDIAN
#       define BIG_ENDIAN_PLATFORM 1
#   else
#       error "Unknown platform endianess"
#   endif
#endif

// /* Machine byte-order, reuse preprocessor provided macros when available */
// #if !defined(__ORDER_BIG_ENDIAN__)
// #  define __ORDER_BIG_ENDIAN__ 4321
// #endif
//
// #if defined(__ORDER_LITTLE_ENDIAN__)
// #  define __ORDER_LITTLE_ENDIAN__ 1234
// #endif
//
// /*
//  * ARM family, known revisions: V5, V6, and V7
//  *
//  * ARM is bi-endian, detect using __ARMEL__ or __ARMEB__, falling back to
//  * auto-detection implemented below.
//  */
// #if defined(__arm__) || defined(__TARGET_ARCH_ARM) || defined(_M_ARM)
// #   if defined(__ARMEL__)
// #       define PFS_BYTE_ORDER PFS_LITTLE_ENDIAN
// #   elif defined(__ARMEB__)
// #       define PFS_BYTE_ORDER PFS_BIG_ENDIAN
// #   else
// #   endif
//
// /*
//  * X86 family, known variants: 32- and 64-bit
//  *
//  * X86 is little-endian.
//  */
// #elif defined(__i386) || defined(__i386__) || defined(_M_IX86)
// #   define PFS_BYTE_ORDER PFS_LITTLE_ENDIAN
// #elif defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(_M_X64)
// #	define PFS_BYTE_ORDER PFS_LITTLE_ENDIAN
//
// /*
//     Itanium (IA-64) family, no revisions or variants
//     Itanium is bi-endian, use endianness auto-detection implemented below.
// */
// #elif defined(__ia64) || defined(__ia64__) || defined(_M_IA64)
// // PFS_BYTE_ORDER not defined, use endianness auto-detection
// /*
//     MIPS family, known revisions: I, II, III, IV, 32, 64
//     MIPS is bi-endian, use endianness auto-detection implemented below.
// */
// #elif defined(__mips) || defined(__mips__) || defined(_M_MRX000)
// #	if defined(__MIPSEL__)
// #   	define PFS_BYTE_ORDER PFS_LITTLE_ENDIAN
// #  	elif defined(__MIPSEB__)
// #   	define PFS_BYTE_ORDER PFS_BIG_ENDIAN
// #  	else
// /* PFS_BYTE_ORDER not defined, use endianness auto-detection */
// # 	endif
//
// /*
//     Power family, known variants: 32- and 64-bit
//     Power is bi-endian, use endianness auto-detection implemented below.
// */
// #elif defined(__ppc__) || defined(__ppc) || defined(__powerpc__) \
//       || defined(_ARCH_COM) || defined(_ARCH_PWR) || defined(_ARCH_PPC)  \
//       || defined(_M_MPPC) || defined(_M_PPC)
// /* PFS_BYTE_ORDER not defined, use endianness auto-detection */
//
//
// /* MS-DOS is an operation system for x86-based personal computers */
// #elif defined(MSDOS) || defined(__MSDOS__) || defined(_MSDOS)
// #	define PFS_BYTE_ORDER PFS_LITTLE_ENDIAN
//
// /* Elbrus 4C - little endian */
// #elif defined(__elbrus_4c__)
// #	define PFS_BYTE_ORDER PFS_LITTLE_ENDIAN
//
// #endif
//
//
// /*
//   NOTE:
//   GCC 4.6 added __BYTE_ORDER__, __ORDER_BIG_ENDIAN__, __ORDER_LITTLE_ENDIAN__
//   and __ORDER_PDP_ENDIAN__ in SVN r165881. If you are using GCC 4.6 or newer,
//   this code will properly detect your target byte order; if you are not, and
//   the __LITTLE_ENDIAN__ or __BIG_ENDIAN__ macros are not defined, then this
//   code will fail to detect the target byte order.
// */
// // Some processors support either endian format, try to detect which we are using.
// #if !defined(PFS_BYTE_ORDER)
// #	if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == PFS_BIG_ENDIAN || __BYTE_ORDER__ == PFS_LITTLE_ENDIAN)
// // Reuse __BYTE_ORDER__ as-is, since our Q_*_ENDIAN #defines match the preprocessor defaults
// #		define PFS_BYTE_ORDER __BYTE_ORDER__
// #	elif defined(__BIG_ENDIAN__)
// #   	define PFS_BYTE_ORDER PFS_BIG_ENDIAN
// #  	elif defined(__LITTLE_ENDIAN__)  \
// 		|| defined(_WIN32_WCE) // Windows CE is always little-endian according to MSDN.
// #  		define PFS_BYTE_ORDER PFS_LITTLE_ENDIAN
// #  	else
// #   	error "Unable to determine byte order!"
// #  	endif
// #endif
