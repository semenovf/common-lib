////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2022 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2021.12.06 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "bits/endian.h"
#include "error.hpp"
#include "filesystem.hpp"
#include "fmt.hpp"
#include <array>
#include <system_error>
#include <istream>
#include <string>
#include <cstdint>
#include <cstring>

namespace pfs {
namespace crypto {

#define PFS__COLIN_PERCIVAL_SHA256_IMPL 1
#define PFS__SYSTEM_GLITCH_SHA256_IMPL 0

struct sha256_digest : std::array<std::uint8_t, 32> {};

inline std::string to_string (sha256_digest const & digest)
{
    std::string result;
    result.reserve(64);

    for (int i = 0; i < 32; i++)
        result += fmt::format("{:02x}", digest[i]);

    return result;
}

}} // namespace pfs::crypto

#if PFS__SYSTEM_GLITCH_SHA256_IMPL
#   include "3rdparty/crypto/system_glitch_sha256.hpp"
#endif // PFS__SYSTEM_GLITCH_SHA256_IMPL

#if PFS__COLIN_PERCIVAL_SHA256_IMPL
#   include "3rdparty/crypto/colin_persival_sha256.hpp"
#endif // PFS__COLIN_PERCIVAL_SHA256_IMPL
