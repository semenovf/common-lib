// A C++ SHA256 implementation.
// https://github.com/System-Glitch/SHA256
// License: MIT

#pragma once
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>

namespace pfs {
namespace crypto {

class sha256
{
private:
    uint8_t  _data[64];
    uint32_t _blocklen;
    uint64_t _bitlen;
    uint32_t _state[8]; //A, B, C, D, E, F, G, H

    static constexpr std::array<uint32_t, 64> K = {
        0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,
        0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
        0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,
        0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
        0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,
        0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
        0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,
        0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
        0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,
        0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
        0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,
        0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
        0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,
        0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
        0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,
        0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
    };

public:
    sha256 ()
        : _blocklen(0)
        , _bitlen(0)
    {
        _state[0] = 0x6a09e667;
        _state[1] = 0xbb67ae85;
        _state[2] = 0x3c6ef372;
        _state[3] = 0xa54ff53a;
        _state[4] = 0x510e527f;
        _state[5] = 0x9b05688c;
        _state[6] = 0x1f83d9ab;
        _state[7] = 0x5be0cd19;
    }

    static sha256_digest digest (std::uint8_t const * src, std::size_t n)
    {
        sha256 hash;
        hash.update(src, n);
        return hash.digest();
    }

    static inline sha256_digest digest (char const * src)
    {
        return digest(reinterpret_cast<std::uint8_t const *>(src), std::strlen(src));
    }

    static inline sha256_digest digest (std::string const & src)
    {
        return digest(reinterpret_cast<std::uint8_t const *>(src.data()), src.size());
    }

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
        std::vector<std::uint8_t> buffer(kBUFSZ);

        sha256 hash;

        auto saved_exceptions = is.exceptions();

        try {
            is.exceptions(std::ios::failbit | std::ios::badbit);
            is.seekg(0, std::ios::beg);

            while (is.good()) {
                is.read(reinterpret_cast<char *>(buffer.data()), kBUFSZ);
                hash.update(buffer.data(), is.gcount());
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

        return hash.digest();
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

private:
    void update (uint8_t const * data, size_t length)
    {
        for (size_t i = 0 ; i < length ; i++) {
            _data[_blocklen++] = data[i];

            if (_blocklen == 64) {
                transform();

                // End of the block
                _bitlen += 512;
                _blocklen = 0;
            }
        }
    }

//     inline void update (std::string const & data)
//     {
//         update(reinterpret_cast<const uint8_t *>(data.c_str()), data.size());
//     }

    sha256_digest digest ()
    {
        sha256_digest hash;
        pad();
        revert(hash.data());
        return hash;
    }

    //static std::string toString(const uint8_t * digest);

    static inline std::uint32_t rotr (std::uint32_t x, std::uint32_t n)
    {
        return (x >> n) | (x << (32 - n));
    }

    static inline std::uint32_t choose (std::uint32_t e, std::uint32_t f, std::uint32_t g)
    {
        return (e & f) ^ (~e & g);
    }

    static inline std::uint32_t majority (std::uint32_t a, std::uint32_t b, std::uint32_t c)
    {
        return (a & (b | c)) | (b & c);
    }

    static inline std::uint32_t sig0 (std::uint32_t x)
    {
        return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
    }

    static inline std::uint32_t sig1 (std::uint32_t x)
    {
        return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
    }

    void transform ()
    {
        std::uint32_t maj, xorA, ch, xorE, sum, newA, newE, m[64];
        std::uint32_t state[8];

        for (std::uint8_t i = 0, j = 0; i < 16; i++, j += 4) { // Split data in 32 bit blocks for the 16 first words
            m[i] = (_data[j] << 24)
                | (_data[j + 1] << 16)
                | (_data[j + 2] << 8)
                | (_data[j + 3]);
        }

        for (std::uint8_t k = 16 ; k < 64; k++) { // Remaining 48 blocks
            m[k] = sig1(m[k - 2]) + m[k - 7] + sig0(m[k - 15]) + m[k - 16];
        }

        for (std::uint8_t i = 0 ; i < 8 ; i++) {
            state[i] = _state[i];
        }

        for (std::uint8_t i = 0; i < 64; i++) {
            maj   = majority(state[0], state[1], state[2]);
            xorA  = rotr(state[0], 2) ^ rotr(state[0], 13) ^ rotr(state[0], 22);

            ch = choose(state[4], state[5], state[6]);

            xorE  = rotr(state[4], 6) ^ rotr(state[4], 11) ^ rotr(state[4], 25);

            sum  = m[i] + K[i] + state[7] + ch + xorE;
            newA = xorA + maj + sum;
            newE = state[3] + sum;

            state[7] = state[6];
            state[6] = state[5];
            state[5] = state[4];
            state[4] = newE;
            state[3] = state[2];
            state[2] = state[1];
            state[1] = state[0];
            state[0] = newA;
        }

        for (std::uint8_t i = 0 ; i < 8 ; i++) {
            _state[i] += state[i];
        }
    }

    void pad ()
    {
        std::uint64_t i = _blocklen;
        std::uint8_t end = _blocklen < 56 ? 56 : 64;

        _data[i++] = 0x80; // Append a bit 1

        while (i < end) {
            _data[i++] = 0x00; // Pad with zeros
        }

        if (_blocklen >= 56) {
            transform();
            std::memset(_data, 0, 56);
        }

        // Append to the padding the total message's length in bits and transform.
        _bitlen += _blocklen * 8;
        _data[63] = _bitlen;
        _data[62] = _bitlen >> 8;
        _data[61] = _bitlen >> 16;
        _data[60] = _bitlen >> 24;
        _data[59] = _bitlen >> 32;
        _data[58] = _bitlen >> 40;
        _data[57] = _bitlen >> 48;
        _data[56] = _bitlen >> 56;
        transform();
    }

    void revert (std::uint8_t * hash)
    {
        // SHA uses big endian byte ordering
        // Revert all bytes
        for (std::uint8_t i = 0 ; i < 4 ; i++) {
            for (std::uint8_t j = 0 ; j < 8 ; j++) {
                hash[i + (j * 4)] = (_state[j] >> (24 - i * 8)) & 0x000000ff;
            }
        }
    }
};

constexpr const std::array<std::uint32_t, 64> sha256::K;

}} // namespace pfs::crypto
