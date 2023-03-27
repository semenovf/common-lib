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
#include <cmath>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>

namespace pfs {

/**
 * @param SizeType Size type for sequences having size (@c string_view,
 *        @c string, @c std::vector<char>)
 */
template <typename SizeType = std::uint32_t, endian Endian = endian::native>
class binary_ostream
{
public:
    using size_type = std::vector<char>::size_type;
    using offset_type = size_type;

private:
    std::vector<char> * _pbuf {nullptr};
    offset_type _off {0};
    bool _has_ownership {false};

public:
    binary_ostream (size_type reserve_bytes = 0)
        : _has_ownership(true)
    {
        _pbuf = new std::vector<char>;

        if (reserve_bytes > 0) {
            _pbuf->reserve(reserve_bytes);
        }
    }

    binary_ostream (std::vector<char> & buf, offset_type offset = 0)
        : _pbuf(& buf)
        , _off(offset)
        , _has_ownership(false)
    {}

    ~binary_ostream ()
    {
        if (_has_ownership) {
            if (_pbuf)
                delete _pbuf;
        }

        _pbuf = nullptr;
    }

    template <typename T>
    inline typename std::enable_if<std::is_integral<T>::value, binary_ostream &>::type
    operator << (T v)
    {
        T a = Endian == endian::network ? to_network_order(v) : v;
        union u { T v; char b[sizeof(T)]; } d;
        d.v = a;

        _pbuf->resize(_pbuf->size() + sizeof(T));
        std::memcpy(_pbuf->data() + _off, d.b, sizeof(T));
        _off += sizeof(T);
        return *this;
    }

    template <typename T>
    inline typename std::enable_if<std::is_enum<T>::value, binary_ostream &>::type
    operator << (T v)
    {
        return this->operator << (static_cast<typename std::underlying_type<T>::type>(v));
    }

    binary_ostream & operator << (float v)
    {
        union { float f; std::uint32_t d; } x;
        x.f = v;
        this->operator << (x.d);

        // static constexpr auto multiplier = (std::numeric_limits<std::int32_t>::max)();
        // int exp = 0;
        // auto mant = static_cast<std::int32_t>(std::frexp(v, & exp) * multiplier);
        // this->operator << (exp);
        // this->operator << (mant);

        return *this;
    }

    binary_ostream & operator << (double v)
    {
        union { double f; std::uint64_t d; } x;
        x.f = v;
        this->operator << (x.d);

        //static constexpr auto multiplier = (std::numeric_limits<std::int64_t>::max)();
        //int exp = 0;
        //auto mant = static_cast<std::int64_t>(std::frexp(v, & exp) * multiplier);
        //this->operator << (exp);
        //this->operator << (mant);

        return *this;
    }

    /**
     * Writes raw sequence into the stream without size.
     *
     * @throw error {std::errc::result_out_of_range} if @a raw points to reverse
     *        sequence.
     */
    binary_ostream & operator << (std::pair<char const *, char const *> const & raw)
    {
        if (raw.first > raw.second)
            throw error { std::make_error_code(std::errc::result_out_of_range) };

        auto nbytes = static_cast<size_type>(raw.second - raw.first);

        if (nbytes > 0) {
            _pbuf->resize(_pbuf->size() + nbytes);
            std::memcpy(_pbuf->data() + _off, raw.first, nbytes);
            _off += nbytes;
        }

        return *this;
    }

    binary_ostream & operator << (std::pair<char const *, size_type> const & raw)
    {
        if (raw.second > 0) {
            _pbuf->resize(_pbuf->size() + raw.second);
            std::memcpy(_pbuf->data() + _off, raw.first, raw.second);
            _off += raw.second;
        }

        return *this;
    }

    /**
     * @throw error {std::errc::value_too_large} if @a sw is too long.
     */
    binary_ostream & operator << (string_view sw)
    {
        if (sw.size() > 0) {
            if ((sw.size() > (std::numeric_limits<SizeType>::max)()))
                throw error { std::make_error_code(std::errc::value_too_large) };

            this->operator << (static_cast<SizeType>((sw.size())));

            _pbuf->resize(_pbuf->size() + sw.size());
            std::memcpy(_pbuf->data() + _off, sw.data(), sw.size());
            _off += sw.size();
        }

        return *this;
    }

    binary_ostream & operator << (char const * s)
    {
        return this->operator << (string_view(s, std::strlen(s)));
    }

    binary_ostream & operator << (std::string const & s)
    {
        return this->operator << (string_view{s});
    }

    binary_ostream & operator << (std::vector<char> const & s)
    {
        return this->operator << (string_view{s.data(), s.size()});
    }

    char const * data () const noexcept
    {
        return _pbuf->data();
    }

    std::size_t size () const noexcept
    {
        return _pbuf->size();
    }
};

} // namespace pfs
