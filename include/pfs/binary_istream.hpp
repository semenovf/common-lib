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
#include <type_traits>

namespace pfs {

template <endian Endian = endian::native>
class binary_istream
{
    char const * _p;
    char const * _end;

public:
    binary_istream (char const * begin, char const * end)
        : _p(begin)
        , _end(end)
    {}

    /**
     * @throws error {std::errc::no_buffer_space} if not enough data to
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
            throw error { std::make_error_code(std::errc::no_buffer_space) };
        }

        return *this;
    }

    /**
     * @throws error {std::errc::no_buffer_space} if not enough data to
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
     * @throws error {std::errc::no_buffer_space} if not enough data to
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
     * @throws error {std::errc::no_buffer_space} if not enough data to
     *         deserialize value.
     */
    binary_istream & operator >> (string_view & v)
    {
        std::int32_t sz = 0;
        this->operator >> (sz);

        if (_p + sz <= _end) {
            v = string_view(_p, sz);
            _p += sz;
        } else {
            throw error { std::make_error_code(std::errc::no_buffer_space) };
        }

        return *this;
    }

    /**
     * @throws error {std::errc::no_buffer_space} if not enough data to
     *         deserialize value.
     */
    binary_istream & operator >> (std::string & v)
    {
        string_view sw;
        this->operator >> (sw);
        v = std::string(sw.data(), sw.size());
        return *this;
    }
};

} // namespace pfs

