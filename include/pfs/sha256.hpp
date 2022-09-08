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
#include "integer.hpp"
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

struct sha256_digest : std::array<std::uint8_t, 32>
{
    sha256_digest () { fill(0); }
    sha256_digest (std::string const & s, std::error_code & ec) noexcept;
    sha256_digest (std::string const & s);
};

inline bool is_valid (sha256_digest const & digest) noexcept
{
    return digest != sha256_digest{};
}

inline std::string to_string (sha256_digest const & digest)
{
    std::string result;
    result.reserve(64);

    for (int i = 0; i < 32; i++)
        result += fmt::format("{:02x}", digest[i]);

    return result;
}

inline sha256_digest to_sha256_digest_unsafe (char const * s, std::error_code & ec)
{
    sha256_digest result;

    for (int i = 0; !ec && i < 32; i++) {
        result[i] = to_integer<std::uint8_t>(s, s + 2, 16, ec);
        s += 2;
    }

    return result;
}

inline sha256_digest to_sha256_digest (char const * s, std::error_code & ec)
{
    if (std::strlen(s) != 64) {
        ec = make_error_code(std::errc::invalid_argument);
        return sha256_digest{};
    }

    return to_sha256_digest_unsafe(s, ec);
}

/**
 * @return SHA256 digest converted from string @a s. On failure @a ec set to
 *         @c std::errc::invalid_argument.
 */
inline sha256_digest to_sha256_digest (std::string const & s, std::error_code & ec)
{
    if (s.size() != 64) {
        ec = make_error_code(std::errc::invalid_argument);
        return sha256_digest{};
    }

    return to_sha256_digest_unsafe(s.c_str(), ec);
}

inline sha256_digest::sha256_digest (std::string const & s, std::error_code & ec) noexcept
{
    *this = to_sha256_digest(s, ec);
}

inline sha256_digest::sha256_digest (std::string const & s)
{
    std::error_code ec;
    *this = to_sha256_digest(s, ec);

    if (ec)
        throw error{ec};
}

}} // namespace pfs::crypto

//#include "3rdparty/crypto/colin_persival_sha256_mod.hpp"
#include "3rdparty/crypto/colin_persival_sha256.hpp"

namespace pfs {
namespace crypto {

class sha256
{
public:
    static inline sha256_digest digest (std::uint8_t const * src
        , std::size_t n) noexcept
    {
        return details::sha256::digest(src, n);
    }

    static inline sha256_digest digest (char const * src) noexcept
    {
        return digest(reinterpret_cast<std::uint8_t const *>(src), std::strlen(src));
    }

    static inline sha256_digest digest (std::string const & src) noexcept
    {
        return digest(reinterpret_cast<std::uint8_t const *>(src.data()), src.size());
    }

    static inline sha256_digest digest (std::istream & is, std::error_code & ec) noexcept
    {
        return details::sha256::digest(is, ec);
    }

    /**
     * @throws error with @c std::ios_base::failure error codes.
     */
    static sha256_digest digest (std::istream & is)
    {
        std::error_code ec;
        auto res = digest(is, ec);

        if (ec)
            throw error(ec);

        return res;
    }

    /**
     */
    static sha256_digest digest (filesystem::path const & path, std::error_code & ec) noexcept
    {
        if (!filesystem::exists(path)) {
            ec = std::make_error_code(std::errc::no_such_file_or_directory);
            return sha256_digest{};
        }

        std::ifstream ifs{filesystem::utf8_encode(path), std::ios::binary};
        return digest(ifs, ec);
    }

    /**
     * @throws error @c std::errc::no_such_file_or_directory.
     * @throws error with @c std::ios_base::failure error codes.
     */
    static sha256_digest digest (filesystem::path const & path)
    {
        std::error_code ec;
        auto res = digest(path, ec);

        if (ec)
            throw error(ec);

        return res;
    }
};

}} // namespace pfs::crypto
