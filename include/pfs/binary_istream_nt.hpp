////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020-2024 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2024.05.07 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <pfs/binary_istream.hpp>
#include <cstdint>
#include <functional>
#include <type_traits>
#include <utility>
#include <vector>

namespace pfs {

#ifndef PFS__EXPECTED_SIZE_DEFINED
#   define PFS__EXPECTED_SIZE_DEFINED 1
    struct expected_size
    {
        std::uint32_t sz;
    };
#endif

/**
 * No-throw version of @c binary_istream class.
 */
template <endian Endianess = endian::native>
class binary_istream_nt
{
public:
    using size_type = std::uint32_t;

    enum status_enum {
          good
        , out_of_bound
        , corrupted
    };

private:
    char const * _p {nullptr};
    char const * _end {nullptr};
    optional<size_type> _expected_size;
    status_enum _state {good};

public:
    binary_istream_nt (char const * begin, char const * end)
        : _p(begin)
        , _end(end)
    {
        if (_p == nullptr || _end == nullptr)
            throw error {make_error_code(std::errc::invalid_argument)};

        if (_p > _end)
            throw error {make_error_code(std::errc::invalid_argument)};
    }

    binary_istream_nt (char const * begin, std::size_t size)
        : _p(begin)
        , _end(begin + numeric_cast<size_type>(size))
    {
        if (_p == nullptr)
            throw error {make_error_code(std::errc::invalid_argument)};
    }

    char const * begin () const noexcept
    {
        return _p;
    }

    bool empty () const noexcept
    {
        return _p == _end;
    }

    bool is_good () const noexcept
    {
        return _state == good;
    }

    operator bool () const noexcept
    {
        return !this->empty() && _state == good;
    }

    size_type available () const noexcept
    {
        return numeric_cast<size_type>(_end - _p);
    }

    /**
     * Skips @a nbytes in stream.
     */
    void skip (size_type nbytes)
    {
        if (_state == status_enum::good) {
            if (this->_p + nbytes <= this->_end) {
                this->_p += nbytes;
            } else {
                _state = status_enum::out_of_bound;
            }
        }
    }

    template <typename T>
    binary_istream_nt & operator >> (T & v)
    {
        unpack(*this, v);
        return *this;
    }

    binary_istream_nt & operator >> (std::pair<char *, size_type> raw)
    {
        unpack(*this, raw);
        return *this;
    }

    binary_istream_nt & operator >> (expected_size esz)
    {
        _expected_size = esz.sz;
        return *this;
    }

private:
    template <typename T>
    friend typename std::enable_if<std::is_integral<T>::value, void>::type
    unpack (binary_istream_nt & in, T & v)
    {
        if (in._state == status_enum::good) {
            if (in._p + sizeof(T) <= in._end) {
                T const * p = reinterpret_cast<T const *>(in._p);
                v = Endianess == endian::network ? to_native_order(*p) : *p;
                in._p += sizeof(T);
            } else {
                in._state = status_enum::out_of_bound;
            }
        }
    }

    template <typename T>
    friend typename std::enable_if<std::is_enum<T>::value, void>::type
    unpack (binary_istream_nt & in, T & v)
    {

        typename std::underlying_type<T>::type tmp;
        unpack(in, tmp);

        if (in._state == status_enum::good)
            v = static_cast<T>(tmp);
    }

    friend void unpack (binary_istream_nt & in, float & v)
    {
        union { float f; std::uint32_t d; } x;
        unpack(in, x.d);

        if (in._state == status_enum::good)
            v = x.f;
    }

    /**
     */
    friend void unpack (binary_istream_nt & in, double & v)
    {
        union { double f; std::uint64_t d; } x;
        unpack(in, x.d);

        if (in._state == status_enum::good)
            v = x.f;
    }

    /**
     * Reads byte sequence into string view. Lifetime of the string view must be less or equals to
     * the stream's lifetime
     *
     * @throws error {std::errc::result_out_of_range} if not enough data to
     *         deserialize value.
     */
    friend void unpack (binary_istream_nt & in, string_view & v)
    {
        if (in._state == status_enum::good) {
            size_type sz = 0;

            if (in._expected_size)
                sz = *in._expected_size;
            else
                unpack(in, sz);

            if (in._state == status_enum::good) {
                if (sz > 0) {
                    if (in._p + sz <= in._end) {
                        v = string_view(in._p, sz);
                        in._p += sz;
                    } else {
                        in._state = binary_istream_nt::out_of_bound;
                    }
                }
            }
        }

        in._expected_size = nullopt;
    }

    friend void unpack (binary_istream_nt & in, std::string & v)
    {
        string_view sw;
        unpack(in, sw);

        if (in._state == status_enum::good)
            v = std::string(sw.data(), sw.size());
    }

    template <typename Char>
    static void unpack_helper (binary_istream_nt & in, std::vector<Char> & v)
    {
        // No data left
        if (in._p == in._end)
            return;

        if (in._state == status_enum::good) {
            size_type sz = 0;

            if (in._expected_size)
                sz = *in._expected_size;
            else
                unpack(in, sz);

            if (in._state == status_enum::good) {
                if (sz > 0) {
                    if (in._p + sz <= in._end) {
                        v.resize(sz);
                        std::memcpy(reinterpret_cast<Char *>(v.data()), in._p, sz);
                        in._p += sz;
                    } else {
                        in._state == status_enum::out_of_bound;
                    }
                }
            }
        }

        in._expected_size = nullopt;
    }

    friend void unpack (binary_istream_nt & in, std::vector<char> & v)
    {
        binary_istream_nt<Endianess>::unpack_helper(in, v);
    }

    friend void unpack (binary_istream_nt & in, std::vector<std::uint8_t> & v)
    {
        binary_istream_nt<Endianess>::unpack_helper(in, v);
    }

    /**
     * @throws error {std::errc::not_enough_memory} if array is smaller than expected size.
     */
    template <std::size_t N>
    friend void unpack (binary_istream_nt & in, std::array<char, N> & v)
    {
        if (in._state == status_enum::good) {
            size_type sz = 0;

            if (in._expected_size)
                sz = *in._expected_size;
            else
                unpack(in, sz);

            if (sz > N)
                throw error { std::make_error_code(std::errc::not_enough_memory) };

            if (in._state == status_enum::good) {
                if (sz > 0) {
                    if (in._p + sz <= in._end) {
                        std::copy(in._p, in._p + sz, v.data());
                        in._p += sz;
                    } else {
                        in._state == status_enum::out_of_bound;
                    }
                }
            }
        }

        in._expected_size = nullopt;
    }

    friend void unpack (binary_istream_nt & in, std::pair<char *, size_type> raw)
    {
        if (in._state == status_enum::good) {
            size_type sz = 0;

            if (in._expected_size)
                sz = *in._expected_size;
            else
                unpack(in, sz);

            if (in._state == status_enum::good) {
                if (sz > raw.second)
                    throw error { std::make_error_code(std::errc::not_enough_memory) };

                if (sz > 0) {
                    if (in._p + sz <= in._end) {
                        std::copy(in._p, in._p + sz, raw.first);
                        in._p += sz;
                    } else {
                        in._state == status_enum::out_of_bound;
                    }
                }
            }
        }

        in._expected_size = nullopt;
    }
};

} // namespace pfs


