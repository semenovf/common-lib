////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2021.10.14 Initial version.
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/bits/compiler.h"
#include "pfs/endian.hpp"

#if PFS__COMPILER_GCC
#   include <arpa/inet.h> // for htons/htonl and vice versa
#   define __HAS_HTONx__ 1
#endif

#if PFS__COMPILER_MSC || PFS__COMPILER_CLANG_MSC
#   include <winsock.h> // for htons/htonl and vice versa
#   define __HAS_HTONx__ 1
#endif

TEST_CASE("endian") {
    CHECK_EQ(pfs::to_network_order(pfs::to_network_order(std::uint16_t{1234})), 1234);
    CHECK_EQ(pfs::to_native_order(pfs::to_native_order(std::uint16_t{1234})), 1234);

#if __HAS_HTONx__
    CHECK_EQ(pfs::to_network_order(std::uint16_t{1234}), htons(std::uint16_t{1234}));
    CHECK_EQ(pfs::to_native_order(std::uint16_t{1234}), ntohs(std::uint16_t{1234}));

    CHECK_EQ(pfs::to_network_order(std::uint32_t{1234}), htonl(std::uint32_t{1234}));
    CHECK_EQ(pfs::to_native_order(std::uint32_t{1234}), ntohl(std::uint32_t{1234}));
#endif
}
