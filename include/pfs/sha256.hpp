////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2022 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2021.12.06 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "3rdparty/crypto/colin_persival_sha256.hpp"
#include "integer.hpp"
#include "error.hpp"
#include "filesystem.hpp"
#include "fmt.hpp"
#include <cstdint>
#include <cstring>
#include <array>
#include <fstream>
#include <istream>
#include <string>
#include <system_error>
#include <vector>

namespace pfs {
namespace crypto {

struct sha256_digest : std::array<std::uint8_t, 32>
{
    sha256_digest () { fill(0); }
    sha256_digest (std::string const & s, std::error_code & ec) noexcept;
    sha256_digest (std::string const & s);
};

class sha256
{
public:
    using batch_context = details::batch_context;
    using batch_context_ptr = std::unique_ptr<batch_context>;

private:
    details::sha256 _hash;

public:
    sha256 () = default;

    void update (std::uint8_t const * chunk, std::size_t len)
    {
        _hash.update(chunk, len);
    }

    void update (char const * chunk, std::size_t len)
    {
        _hash.update(reinterpret_cast<std::uint8_t const *>(chunk), len);
    }

    sha256_digest digest ()
    {
        sha256_digest result;
        _hash.final(result.data());
        return result;
    }

public: // static
    static inline sha256_digest digest (std::uint8_t const * src, std::size_t n) noexcept
    {
        sha256 hash;
        hash.update(src, n);
        return hash.digest();
    }

    static inline sha256_digest digest (char const * src, std::size_t n) noexcept
    {
        sha256 hash;
        hash.update(src, n);
        return hash.digest();
    }

    static inline sha256_digest digest (char const * src) noexcept
    {
        return digest(reinterpret_cast<std::uint8_t const *>(src), std::strlen(src));
    }

    static inline sha256_digest digest (std::string const & src) noexcept
    {
        return digest(reinterpret_cast<std::uint8_t const *>(src.data()), src.size());
    }

    static sha256_digest digest (std::istream & is, std::size_t bufsz, std::error_code & ec) noexcept
    {
        sha256 hash;
        std::vector<std::uint8_t> buffer(bufsz);
        auto saved_exceptions = is.exceptions();

        try {
            is.exceptions(std::ios::failbit | std::ios::badbit);
            is.seekg(0, std::ios::beg);

            while (is.good()) {
                is.read(reinterpret_cast<char *>(buffer.data()), buffer.size());
                hash.update(buffer.data(), is.gcount());
            }
        } catch (std::ios_base::failure ex) {
            if (is.eof()) {
                hash.update(buffer.data(), is.gcount());
            } else {
                ec = ex.code();
                is.exceptions(saved_exceptions);
                return sha256_digest{};
            }
        } catch (...) {
            ec = make_error_code(errc::unexpected_error);
            return sha256_digest{};
        }

        is.clear(); // Need to clear any existing error flags first otherwise bellow call throws an exception
        is.exceptions(saved_exceptions);
        return hash.digest();
    }

    static inline sha256_digest digest (std::istream & is, std::error_code & ec) noexcept
    {
        return digest(is, 4096, ec);
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

        std::ifstream ifs;
        ifs.exceptions(std::ios::failbit | std::ios::badbit);

        try {
            ifs.open(pfs::utf8_encode_path(path), std::ios::binary);
        } catch (std::ios_base::failure ex) {
            ec = ex.code();
            return sha256_digest{};
        } catch (...) {
            ec = make_error_code(errc::unexpected_error);
            return sha256_digest{};
        }

        return digest(ifs, ec);
    }

    /**
     * @throws error{std::errc::no_such_file_or_directory}.
     * @throws error{std::ios_base::failure}.
     * @throws error{pfs::errc::unexpected_error} on other failures.
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
