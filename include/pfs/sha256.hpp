////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2021.12.06 Initial version.
////////////////////////////////////////////////////////////////////////////////

/*-
 * Copyright 2005,2007,2009 Colin Percival
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */
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

struct sha256_digest
{
    std::array<std::uint8_t, 32> value;
};

// https://en.wikipedia.org/wiki/SHA-2
class sha256
{
    std::uint64_t _count {0};
    std::uint32_t _state[8];
    std::uint8_t  _buf[64];

private:
    static constexpr std::uint32_t K[64] = {
          0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL, 0x3956c25bUL
        , 0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL, 0xd807aa98UL, 0x12835b01UL
        , 0x243185beUL, 0x550c7dc3UL, 0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL
        , 0xc19bf174UL, 0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL
        , 0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL, 0x983e5152UL
        , 0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL, 0xc6e00bf3UL, 0xd5a79147UL
        , 0x06ca6351UL, 0x14292967UL, 0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL
        , 0x53380d13UL, 0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL
        , 0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL, 0xd192e819UL
        , 0xd6990624UL, 0xf40e3585UL, 0x106aa070UL, 0x19a4c116UL, 0x1e376c08UL
        , 0x2748774cUL, 0x34b0bcb5UL, 0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL
        , 0x682e6ff3UL, 0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL
        , 0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL
    };

private:
    sha256 ()
    {
        _state[0] = 0x6a09e667UL;
        _state[1] = 0xbb67ae85UL;
        _state[2] = 0x3c6ef372UL;
        _state[3] = 0xa54ff53aUL;
        _state[4] = 0x510e527fUL;
        _state[5] = 0x9b05688cUL;
        _state[6] = 0x1f83d9abUL;
        _state[7] = 0x5be0cd19UL;
    }

    static inline void store32_be (std::uint8_t dst[4], std::uint32_t w)
    {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        std::memcpy(dst, & w, sizeof(w));
#else
        dst[3] = static_cast<std::uint8_t>(w); w >>= 8;
        dst[2] = static_cast<std::uint8_t>(w); w >>= 8;
        dst[1] = static_cast<std::uint8_t>(w); w >>= 8;
        dst[0] = static_cast<std::uint8_t>(w);
#endif
    }

    static inline uint32_t load32_be (std::uint8_t const src[4])
    {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        std::uint32_t w;
        std::memcpy(&w, src, sizeof w);
#else
        std::uint32_t w = static_cast<std::uint32_t>(src[3]);
        w |= static_cast<std::uint32_t>(src[2]) <<  8;
        w |= static_cast<std::uint32_t>(src[1]) << 16;
        w |= static_cast<std::uint32_t>(src[0]) << 24;

#endif
        return w;
    }

    static inline void store64_be (std::uint8_t dst[8], std::uint64_t w)
    {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        std::memcpy(& dst[0], & w, sizeof(w));
#else
        dst[7] = static_cast<std::uint8_t>(w); w >>= 8;
        dst[6] = static_cast<std::uint8_t>(w); w >>= 8;
        dst[5] = static_cast<std::uint8_t>(w); w >>= 8;
        dst[4] = static_cast<std::uint8_t>(w); w >>= 8;
        dst[3] = static_cast<std::uint8_t>(w); w >>= 8;
        dst[2] = static_cast<std::uint8_t>(w); w >>= 8;
        dst[1] = static_cast<std::uint8_t>(w); w >>= 8;
        dst[0] = static_cast<std::uint8_t>(w);
#endif
    }

    static inline void be32enc_vect (std::uint8_t * dst
        , std::uint32_t const * src
        , std::size_t len)
    {
        for (std::size_t i = 0; i < len / 4; i++)
            store32_be(dst + i * 4, src[i]);
    }

    static inline void be32dec_vect (std::uint32_t * dst
        , std::uint8_t const * src
        , std::size_t len)
    {
        for (std::size_t i = 0; i < len / 4; i++)
            dst[i] = load32_be(src + i * 4);
    }

    static constexpr std::uint32_t rotr32 (std::uint32_t const x, int const b)
    {
        return (x >> b) | (x << (32 - b));
    }

    static constexpr std::uint32_t majority (std::uint32_t x
        , std::uint32_t y
        , std::uint32_t z)
    {
        return (x & (y | z)) | (y & z);
    }

    static constexpr std::uint32_t choose (std::uint32_t x
        , std::uint32_t y
        , std::uint32_t z)
    {
        return (x & (y ^ z)) ^ z;
    }

    static constexpr std::uint32_t Sig0 (std::uint32_t x)
    {
        return rotr32(x, 2) ^ rotr32(x, 13) ^ rotr32(x, 22);
    }

    static constexpr std::uint32_t Sig1 (std::uint32_t x)
    {
        return rotr32(x, 6) ^ rotr32(x, 11) ^ rotr32(x, 25);
    }

    static constexpr std::uint32_t sig0 (std::uint32_t x)
    {
        return rotr32(x, 7) ^ rotr32(x, 18) ^ (x >> 3);
    }

    static constexpr std::uint32_t sig1 (std::uint32_t x)
    {
        return rotr32(x, 17) ^ rotr32(x, 19) ^ (x >> 10);
    }

#define SHA256_RND(a, b, c, d, e, f, g, h, k)                                  \
    h += Sig1(e) + choose(e, f, g) + k;                                        \
    d += h;                                                                    \
    h += Sig0(a) + majority(a, b, c);

#define SHA256_RNDr(S, W, i, ii)                                               \
    SHA256_RND(S[(64 - i) % 8], S[(65 - i) % 8], S[(66 - i) % 8]               \
        , S[(67 - i) % 8], S[(68 - i) % 8], S[(69 - i) % 8]                    \
        , S[(70 - i) % 8], S[(71 - i) % 8], W[i + ii] + K[i + ii])

#define SHA256_MSCH(W, ii, i)                                                  \
    W[i + ii + 16] =                                                           \
        sig1(W[i + ii + 14]) + W[i + ii + 9] + sig0(W[i + ii + 1]) + W[i + ii]

    void transform (std::uint32_t W[64], std::uint32_t S[8])
    {
        be32dec_vect(W, _buf, 64);
        std::memcpy(S, _state, 32);

        for (int i = 0; i < 64; i += 16) {
            SHA256_RNDr(S, W, 0, i);
            SHA256_RNDr(S, W, 1, i);
            SHA256_RNDr(S, W, 2, i);
            SHA256_RNDr(S, W, 3, i);
            SHA256_RNDr(S, W, 4, i);
            SHA256_RNDr(S, W, 5, i);
            SHA256_RNDr(S, W, 6, i);
            SHA256_RNDr(S, W, 7, i);
            SHA256_RNDr(S, W, 8, i);
            SHA256_RNDr(S, W, 9, i);
            SHA256_RNDr(S, W, 10, i);
            SHA256_RNDr(S, W, 11, i);
            SHA256_RNDr(S, W, 12, i);
            SHA256_RNDr(S, W, 13, i);
            SHA256_RNDr(S, W, 14, i);
            SHA256_RNDr(S, W, 15, i);

            if (i == 48)
                break;

            SHA256_MSCH(W, 0, i);
            SHA256_MSCH(W, 1, i);
            SHA256_MSCH(W, 2, i);
            SHA256_MSCH(W, 3, i);
            SHA256_MSCH(W, 4, i);
            SHA256_MSCH(W, 5, i);
            SHA256_MSCH(W, 6, i);
            SHA256_MSCH(W, 7, i);
            SHA256_MSCH(W, 8, i);
            SHA256_MSCH(W, 9, i);
            SHA256_MSCH(W, 10, i);
            SHA256_MSCH(W, 11, i);
            SHA256_MSCH(W, 12, i);
            SHA256_MSCH(W, 13, i);
            SHA256_MSCH(W, 14, i);
            SHA256_MSCH(W, 15, i);
        }

        for (int i = 0; i < 8; i++)
            _state[i] += S[i];
    }

#undef SHA256_RND
#undef SHA256_RNDr
#undef SHA256_MSCH

    void pad (std::uint32_t tmp32[64 + 8])
    {
        std::uint64_t r = (_count >> 3) & 0x3f;

        if (r < 56) {
            _buf[r + 0] = 0x80;

            for (int i = 1; i < 56 - r; i++)
                _buf[r + i] = 0x00;
        } else {
            _buf[r + 0] = 0x80;

            for (int i = 1; i < 64 - r; i++)
                _buf[r + i] = 0x00;

            transform(& tmp32[0], & tmp32[64]);
            std::memset(& _buf[0], 0, 56);
        }

        store64_be(& _buf[56], _count);
        transform(& tmp32[0], & tmp32[64]);
    }

    void update (std::uint8_t const * in, std::size_t inlen)
    {
        std::uint32_t tmp32[64 + 8];

        std::uint64_t r = (_count >> 3) & 0x3f;
        _count += static_cast<std::uint64_t>(inlen) << 3;

        if (inlen < 64 - r) {
            for (int i = 0; i < inlen; i++)
                _buf[r + i] = in[i];

            return;
        }

        for (int i = 0; i < 64 - r; i++)
            _buf[r + i] = in[i];

        transform(& tmp32[0], & tmp32[64]);

        in += 64 - r;
        inlen -= 64 - r;

        while (inlen >= 64) {
            transform(& tmp32[0], & tmp32[64]);
            in += 64;
            inlen -= 64;
        }

        inlen &= 63;

        for (int i = 0; i < inlen; i++)
            _buf[i] = in[i];

        std::memset(& tmp32[0], 0, sizeof(tmp32));
    }

    int final (std::uint8_t * out)
    {
        std::uint32_t tmp32[64 + 8];

        pad(tmp32);
        be32enc_vect(out, _state, 32);

        std::memset(& tmp32[0], 0, sizeof(tmp32));
        std::memset(& _state[0], 0, sizeof(_state));

        return 0;
    }

public:
    static sha256_digest digest (std::uint8_t const * src, std::size_t n)
    {
        sha256 hash;
        hash.update(src, n);

        sha256_digest result;
        hash.final(result.value.data());
        return result;
    }

    static inline sha256_digest digest (std::string const & src)
    {
        return digest(reinterpret_cast<std::uint8_t const *>(src.data()), src.size());
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

    static sha256_digest digest (std::istream & is, std::error_code & ec) noexcept
    {
        constexpr std::size_t kBUFSZ = 256;
        char buffer[kBUFSZ];

        sha256 hash;

        auto saved_exceptions = is.exceptions();

        try {
            is.exceptions(std::ios::failbit | std::ios::badbit);
            is.seekg(0, std::ios::beg);

            while (is.good()) {
                is.read(buffer, kBUFSZ);
                hash.update(reinterpret_cast<uint8_t *>(buffer), is.gcount());
            }
        } catch (std::ios_base::failure ex) {
            if (!is.eof()) {
                ec = ex.code();
                is.exceptions(saved_exceptions);
                return sha256_digest{};
            }
        } catch (...) {
            ec = make_error_code(errc::unexpected_error);
            return sha256_digest{};
        }

        is.exceptions(saved_exceptions);

        sha256_digest result;
        hash.final(result.value.data());
        return result;
    }
};

constexpr const std::uint32_t sha256::K[64];

inline std::string to_string (sha256_digest const & digest)
{
    std::string result;
    result.reserve(64);

    for (int i = 0; i < 32; i++)
        result += fmt::format("{:02x}", digest.value[i]);

    return result;
}

}} // namespace pfs::crypto
