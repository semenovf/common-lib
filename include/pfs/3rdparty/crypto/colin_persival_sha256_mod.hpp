/*-
 * Copyright 2005 Colin Percival
 * Copyright 2013 Christian Mehlis & René Kijewski
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
 * $FreeBSD: src/lib/libmd/sha256.h,v 1.1.2.1 2005/06/24 13:32:25 cperciva Exp $
 */

/**
 * @defgroup    sys_sha256 SHA264
 * @ingroup     sys
 * @brief       SHA264 hash generator
 */

/**
 * @ingroup     sys_crypto
 *
 * @file        sha256.h
 * @brief       Header definitions for the SHA256 hash function
 *
 * @author      Colin Percival
 * @author      Christian Mehlis
 * @author      Rene Kijewski
 */

#pragma once
#include "pfs/endian.hpp"
#include <cstdint>
#include <cstring>

namespace pfs {
namespace crypto {
namespace details {

// https://en.wikipedia.org/wiki/SHA-2
class sha256
{
    std::uint32_t _state[8]; // global state
    std::uint32_t _count[2]; // processed bytes counter
    std::uint8_t  _buf[64];  // data buffer

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
        // Zero bits processed so far
        _count[0] = _count[1] = 0;

        // Magic initialization constants
        _state[0] = 0x6a09e667UL;
        _state[1] = 0xbb67ae85UL;
        _state[2] = 0x3c6ef372UL;
        _state[3] = 0xa54ff53aUL;
        _state[4] = 0x510e527fUL;
        _state[5] = 0x9b05688cUL;
        _state[6] = 0x1f83d9abUL;
        _state[7] = 0x5be0cd19UL;
    }

    ~sha256 ()
    {
        /* Clear the context state */
        std::memset(& _state[0], 0, sizeof(_state));
        std::memset(& _count[0], 0, sizeof(_count));
        std::memset(& _buf[0]  , 0, sizeof(_buf));
    }

private:
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    static inline void be32enc_vect (void * dest, void const * src, std::size_t n)
    {
        std::memcpy(dest, src, n);
    }

    /* Copy a vector of bytes into a vector of big-endian uint32_t */
    static inline void be32dec_vect (void * dest, void const * src, std::size_t n)
    {
        std::memcpy(dest, src, n);
    }
#else /* !__BIG_ENDIAN__ */

    /*
     * Encode a length len/4 vector of (uint32_t) into a length len vector of
     * (unsigned char) in big-endian form.  Assumes len is a multiple of 4.
     */
    static void be32enc_vect (void * dest, void const * src, std::size_t len)
    {
        std::uint32_t * d = static_cast<std::uint32_t *>(dest);
        std::uint32_t const * s = static_cast<std::uint32_t const *>(src);

        for (std::size_t i = 0; i < len / 4; i++) {
            d[i] = pfs::byteswap(s[i]);
        }
    }

    /*
     * Decode a big-endian length len vector of (unsigned char) into a length
     * len/4 vector of (uint32_t).  Assumes len is a multiple of 4.
     */
    static inline void be32dec_vect (void * dest, void const * src, std::size_t n)
    {
        be32enc_vect(dest, src, n);
    }

#endif /* __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__ */

    static constexpr std::uint32_t rotr32 (std::uint32_t const x, int const n)
    {
        return (x >> n) | (x << (32 - n));
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

    /*
     * SHA256 block compression function.  The 256-bit state is transformed via
     * the 512-bit input block to produce a new state.
     */
    void transform (std::uint32_t * state, std::uint8_t const block[64])
    {
        std::uint32_t W[64];
        std::uint32_t S[8];
        int i;

        /* 1. Prepare message schedule W. */
        be32dec_vect(W, block, 64);

        for (i = 16; i < 64; i++) {
            W[i] = sig1(W[i - 2]) + W[i - 7] + sig0(W[i - 15]) + W[i - 16];
        }

        /* 2. Initialize working variables. */
        std::memcpy(S, state, 32);

        /* 3. Mix. */
        for (i = 0; i < 64; ++i) {
            uint32_t e = S[(68 - i) % 8], f = S[(69 - i) % 8];
            uint32_t g = S[(70 - i) % 8], h = S[(71 - i) % 8];
            uint32_t t0 = h + Sig1(e) + choose(e, f, g) + W[i] + K[i];

            uint32_t a = S[(64 - i) % 8], b = S[(65 - i) % 8];
            uint32_t c = S[(66 - i) % 8], d = S[(67 - i) % 8];
            uint32_t t1 = Sig0(a) + majority(a, b, c);

            S[(67 - i) % 8] = d + t0;
            S[(71 - i) % 8] = t0 + t1;
        }

        /* 4. Mix local working variables into global state */
        for (i = 0; i < 8; i++) {
            state[i] += S[i];
        }
    }

    /* Add padding and terminating bit-count. */
    void pad ()
    {
        const unsigned char PAD[64] = {
            0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        };

        /*
         * Convert length to a vector of bytes -- we do this now rather
         * than later because the length will change after we pad.
         */
        std::uint8_t len[8];
        be32enc_vect(len, _count, 8);

        /* Add 1--64 bytes so that the resulting length is 56 mod 64 */
        std::uint32_t r = (_count[1] >> 3) & 0x3f;
        std::uint32_t plen = (r < 56) ? (56 - r) : (120 - r);
        update(PAD, (size_t) plen);

        /* Add the terminating bit-count */
        update(len, 8);
    }

    /* Add bytes into the hash */
    void update (void const * in, size_t len)
    {
        /* Number of bytes left in the buffer from previous updates */
        std::uint32_t r = (_count[1] >> 3) & 0x3f;

        /* Convert the length into a number of bits */
        std::uint32_t bitlen1 = ((std::uint32_t) len) << 3;
        std::uint32_t bitlen0 = ((std::uint32_t) len) >> 29;

        /* Update number of bits */
        if ((_count[1] += bitlen1) < bitlen1) {
            _count[0]++;
        }

        _count[0] += bitlen0;

        /* Handle the case where we don't need to perform any transforms */
        if (len < 64 - r) {
            std::memcpy(& _buf[r], in, len);
            return;
        }

        /* Finish the current block */
        auto src = static_cast<std::uint8_t const *>(in);

        std::memcpy(& _buf[r], src, 64 - r);
        transform(_state, _buf);

        src += 64 - r;
        len -= 64 - r;

        /* Perform complete blocks */
        while (len >= 64) {
            transform(_state, src);
            src += 64;
            len -= 64;
        }

        /* Copy left over data into buffer */
        std::memcpy(_buf, src, len);
    }

    /*
     * SHA-256 finalization.  Pads the input data, exports the hash value,
     * and clears the context state.
     */
    void final (std::uint8_t digest[32]) noexcept
    {
        /* Add padding */
        pad();

        /* Write the hash */
        be32enc_vect(digest, _state, 32);
    }

public:
    static sha256_digest digest (std::uint8_t const * src, std::size_t n) noexcept
    {
        sha256 hash;
        hash.update(src, n);

        sha256_digest result;
        hash.final(result.data());
        return result;
    }

    static sha256_digest digest (std::istream & is, std::error_code & ec) noexcept
    {
        constexpr std::size_t kBUFSZ = 1024;
        std::vector<char> buffer(kBUFSZ);

        sha256 hash;
        auto saved_exceptions = is.exceptions();

        try {
            is.exceptions(std::ios::failbit | std::ios::badbit);

            while (is.good()) {
                is.read(buffer.data(), buffer.size());
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

        is.exceptions(saved_exceptions);

        sha256_digest result;
        hash.final(result.data());
        return result;
    }
};

constexpr const std::uint32_t sha256::K[64];

}}} // namespace pfs::crypto::details
