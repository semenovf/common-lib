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
#include "string_view.hpp"
#include <cstdint>
#include <functional>
#include <type_traits>
#include <utility>
#include <vector>

namespace pfs {

/**
 * @param SizeType
 */
template <endian Endian = endian::native>
class binary_istream
{
public:
    using size_type = std::size_t;

    /// Size type for sequences having
    /// size (@c string_view, @c string, @c std::vector<char>)
    using sequence_size_type = std::uint32_t;

private:
    char const * _p;
    char const * _end;

public:
    binary_istream (char const * begin, char const * end)
        : _p(begin)
        , _end(end)
    {}

    binary_istream (char const * begin, std::size_t size)
        : _p(begin)
        , _end(begin + size)
    {}

    char const * begin () const noexcept
    {
        return _p;
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

    /**
     * @throws error {std::errc::result_out_of_range} if not enough data to
     *         deserialize value.
     */
    template <typename T>
    inline typename std::enable_if<std::is_integral<T>::value, binary_istream &>::type
    operator >> (T & v)
    {
        if (_p + sizeof(T) <= _end) {
            T const * p = reinterpret_cast<T const *>(_p);
            v = Endian == endian::network ? to_native_order(*p) : *p;
            _p += sizeof(T);
        } else {
            throw error { std::make_error_code(std::errc::result_out_of_range) };
        }

        return *this;
    }

    template <typename T>
    inline typename std::enable_if<std::is_enum<T>::value, binary_istream &>::type
    operator >> (T & v)
    {
        typename std::underlying_type<T>::type tmp;
        this->operator >> (tmp);
        v = static_cast<T>(tmp);
        return *this;
    }

    /**
     * @throws error {std::errc::result_out_of_range} if not enough data to
     *         deserialize value.
     */
    binary_istream & operator >> (float & v)
    {
        union { float f; std::uint32_t d; } x;
        this->operator >> (x.d);
        v = x.f;

        // static constexpr auto divider = (std::numeric_limits<std::int32_t>::max)();
        // std::int32_t exp {0};
        // std::int32_t mant {0};
        // this->operator >> (exp);
        // this->operator >> (mant);
        // v = std::ldexp(static_cast<float>(mant) / divider, exp);

        return *this;
    }

    /**
     * @throws error {std::errc::result_out_of_range} if not enough data to
     *         deserialize value.
     */
    binary_istream & operator >> (double & v)
    {
        union { double f; std::uint64_t d; } x;
        this->operator >> (x.d);
        v = x.f;

        // static constexpr auto divider = (std::numeric_limits<std::int64_t>::max)();
        // std::int64_t exp {0};
        // std::int64_t mant {0};
        // this->operator >> (exp);
        // this->operator >> (mant);
        // v = std::ldexp(static_cast<double>(mant) / divider, exp);

        return *this;
    }

    /**
     * @throws error {std::errc::result_out_of_range} if not enough data to
     *         deserialize value.
     */
    binary_istream & operator >> (string_view & v)
    {
        sequence_size_type sz = 0;
        this->operator >> (sz);

        if (_p + sz <= _end) {
            v = string_view(_p, sz);
            _p += sz;
        } else {
            throw error { std::make_error_code(std::errc::result_out_of_range) };
        }

        return *this;
    }

    /**
     * Reads specified by @a v.first number of bytes into @a v.second.
     *
     * @throws error {std::errc::result_out_of_range} if not enough data to
     *         deserialize value.
     */
    binary_istream & operator >> (std::pair<sequence_size_type, std::reference_wrapper<string_view>> && v)
    {
        sequence_size_type sz = v.first;

        if (_p + sz <= _end) {
            v.second.get() = string_view(_p, sz);
            _p += sz;
        } else {
            throw error { std::make_error_code(std::errc::result_out_of_range) };
        }

        return *this;
    }

    /**
     * @throws error {std::errc::result_out_of_range} if not enough data to
     *         deserialize value.
     */
    binary_istream & operator >> (std::string & v)
    {
        string_view sw;
        this->operator >> (sw);
        v = std::string(sw.data(), sw.size());
        return *this;
    }

    binary_istream & operator >> (std::pair<sequence_size_type, std::reference_wrapper<std::string>> && v)
    {
        string_view sw;
        this->operator >> (std::make_pair(v.first, std::ref(sw)));
        v.second.get() = std::string(sw.data(), sw.size());
        return *this;
    }

    binary_istream & operator >> (std::vector<char> & v)
    {
        sequence_size_type sz = 0;
        this->operator >> (sz);

        if (_p + sz <= _end) {
            v.resize(sz);
            std::memcpy(v.data(), _p, sz);
            _p += sz;
        } else {
            throw error { std::make_error_code(std::errc::result_out_of_range) };
        }

        return *this;
    }

    binary_istream & operator >> (std::pair<sequence_size_type, std::reference_wrapper<std::vector<char>>> && v)
    {
        sequence_size_type sz = v.first;

        if (_p + sz <= _end) {
            auto & vect = v.second.get();
            vect.resize(sz);
            std::memcpy(vect.data(), _p, sz);
            _p += sz;
        } else {
            throw error { std::make_error_code(std::errc::result_out_of_range) };
        }

        return *this;
    }
};

} // namespace pfs

