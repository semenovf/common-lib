////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020-2023 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2023.03.22 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "error.hpp"
#include "endian.hpp"
#include "numeric_cast.hpp"
#include "optional.hpp"
#include "string_view.hpp"
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

template <endian Endianess = endian::native>
class binary_istream
{
public:
    using size_type = std::uint32_t;

private:
    char const * _p {nullptr};
    char const * _end {nullptr};
    optional<size_type> _expected_size;

public:
    binary_istream (char const * begin, char const * end)
        : _p(begin)
        , _end(end)
    {
        if (_p == nullptr || _end == nullptr)
            throw error {make_error_code(std::errc::invalid_argument)};

        if (_p > _end)
            throw error {make_error_code(std::errc::invalid_argument)};
    }

    binary_istream (char const * begin, std::size_t size)
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

    operator bool () const noexcept
    {
        return !empty();
    }

    size_type available () const noexcept
    {
        return numeric_cast<size_type>(_end - _p);
    }

   /**
     * Peeks character from underlying istream instance.
     *
     * @return Character peek from stream or @c nullopt on failure.
     *
     * @deprecated Will be removed in future versions.
     */
    optional<char> peek () const noexcept
    {
        return _p != _end ? make_optional(*_p) : nullopt;
    }

    /**
     * Skips @a nbytes in stream.
     *
     * @throws error {std::errc::result_out_of_range} if result position is
     *         out of bounds.
     */
    void skip (size_type nbytes)
    {
        if (_p + nbytes <= _end) {
            _p += nbytes;
        } else {
            throw error { std::make_error_code(std::errc::result_out_of_range) };
        }
    }

    template <typename T>
    binary_istream & operator >> (T & v)
    {
        unpack(*this, v);
        return *this;
    }

    binary_istream & operator >> (std::pair<char *, size_type> raw)
    {
        unpack(*this, raw);
        return *this;
    }

    binary_istream & operator >> (expected_size esz)
    {
        _expected_size = esz.sz;
        return *this;
    }

private:
    template <typename T>
    friend typename std::enable_if<std::is_same<typename std::decay<T>::type, bool>::value, void>::type
    unpack (binary_istream & in, T & v)
    {
        if (in._p + sizeof(std::int8_t) <= in._end) {
            auto p = reinterpret_cast<std::int8_t const *>(in._p);
            v = static_cast<bool>(*p);
            in._p += sizeof(std::int8_t);
        } else {
            throw error { std::make_error_code(std::errc::result_out_of_range) };
        }
    }

    /**
     * @throws error {std::errc::result_out_of_range} if not enough data to
     *         deserialize value.
     */
    template <typename T>
    friend typename std::enable_if<std::is_integral<T>::value && !std::is_same<typename std::decay<T>::type, bool>::value, void>::type
    unpack (binary_istream & in, T & v)
    {
        if (in._p + sizeof(T) <= in._end) {
            T const * p = reinterpret_cast<T const *>(in._p);
            v = Endianess == endian::network ? to_native_order(*p) : *p;
            in._p += sizeof(T);
        } else {
            throw error { std::make_error_code(std::errc::result_out_of_range) };
        }
    }

    template <typename T>
    friend typename std::enable_if<std::is_enum<T>::value, void>::type
    unpack (binary_istream & in, T & v)
    {
        typename std::underlying_type<T>::type tmp;
        unpack(in, tmp);
        v = static_cast<T>(tmp);
    }

    /**
     * @throws error {std::errc::result_out_of_range} if not enough data to
     *         deserialize value.
     */
    friend void unpack (binary_istream & in, float & v)
    {
        union { float f; std::uint32_t d; } x;
        unpack(in, x.d);
        v = x.f;

        // static constexpr auto divider = (std::numeric_limits<std::int32_t>::max)();
        // std::int32_t exp {0};
        // std::int32_t mant {0};
        // this->operator >> (exp);
        // this->operator >> (mant);
        // v = std::ldexp(static_cast<float>(mant) / divider, exp);
    }

    /**
     * @throws error {std::errc::result_out_of_range} if not enough data to
     *         deserialize value.
     */
    friend void unpack (binary_istream & in, double & v)
    {
        union { double f; std::uint64_t d; } x;
        unpack(in, x.d);
        v = x.f;

        // static constexpr auto divider = (std::numeric_limits<std::int64_t>::max)();
        // std::int64_t exp {0};
        // std::int64_t mant {0};
        // this->operator >> (exp);
        // this->operator >> (mant);
        // v = std::ldexp(static_cast<double>(mant) / divider, exp);
    }

    /**
     * Reads byte sequence into string view. Lifetime of the string view must be less or equals to
     * the stream's lifetime
     *
     * @throws error {std::errc::result_out_of_range} if not enough data to
     *         deserialize value.
     */
    friend void unpack (binary_istream & in, string_view & v)
    {
        size_type sz = 0;

        if (in._expected_size)
            sz = *in._expected_size;
        else
            unpack(in, sz);

        if (sz > 0) {
            if (in._p + sz <= in._end) {
                v = string_view(in._p, sz);
                in._p += sz;
            } else {
                throw error { std::make_error_code(std::errc::result_out_of_range) };
            }
        }

        in._expected_size = nullopt;
    }

    /**
     * @throws error {std::errc::result_out_of_range} if not enough data to
     *         deserialize value.
     */
    friend void unpack (binary_istream & in, std::string & v)
    {
        string_view sw;
        unpack(in, sw);
        v = std::string(sw.data(), sw.size());
    }

    template <typename Char>
    static void unpack_helper (binary_istream & in, std::vector<Char> & v)
    {
        // No data left
        if (in._p == in._end)
            return;

        size_type sz = 0;

        if (in._expected_size)
            sz = *in._expected_size;
        else
            unpack(in, sz);

        if (sz > 0) {
            if (in._p + sz <= in._end) {
                v.resize(sz);
                std::memcpy(reinterpret_cast<Char *>(v.data()), in._p, sz);
                in._p += sz;
            } else {
                throw error { std::make_error_code(std::errc::result_out_of_range) };
            }
        }

        in._expected_size = nullopt;
    }

    friend void unpack (binary_istream & in, std::vector<char> & v)
    {
        binary_istream<Endianess>::unpack_helper(in, v);
    }

    friend void unpack (binary_istream & in, std::vector<std::uint8_t> & v)
    {
        binary_istream<Endianess>::unpack_helper(in, v);
    }

    /**
     * @throws error {std::errc::not_enough_memory} if array is smaller than expected size.
     * @throws error {std::errc::result_out_of_range} if not enough data to deserialize value.
     */
    template <std::size_t N>
    friend void unpack (binary_istream & in, std::array<char, N> & v)
    {
        size_type sz = 0;

        if (in._expected_size)
            sz = *in._expected_size;
        else
            unpack(in, sz);

        if (sz > N)
            throw error { std::make_error_code(std::errc::not_enough_memory) };

        if (sz > 0) {
            if (in._p + sz <= in._end) {
                std::copy(in._p, in._p + sz, v.data());
                in._p += sz;
            } else {
                throw error { std::make_error_code(std::errc::result_out_of_range) };
            }
        }

        in._expected_size = nullopt;
    }

    friend void unpack (binary_istream & in, std::pair<char *, size_type> raw)
    {
        size_type sz = 0;

        if (in._expected_size)
            sz = *in._expected_size;
        else
            unpack(in, sz);

        if (sz > raw.second)
            throw error { std::make_error_code(std::errc::not_enough_memory) };

        if (sz > 0) {
            if (in._p + sz <= in._end) {
                std::copy(in._p, in._p + sz, raw.first);
                in._p += sz;
            } else {
                throw error { std::make_error_code(std::errc::result_out_of_range) };
            }

            in._expected_size = nullopt;
        }
    }
};

} // namespace pfs

