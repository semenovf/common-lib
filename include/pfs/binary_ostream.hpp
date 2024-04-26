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
#include "string_view.hpp"
#include <cmath>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>

namespace pfs {

struct exclude_size {};

template <endian Endianess = endian::native>
class binary_ostream
{
public:
    using size_type = std::uint32_t;
    using offset_type = size_type;

private:
    std::vector<char> * _pbuf {nullptr};
    offset_type _off {0};
    bool _has_ownership {false};
    bool _exclude_size {false};

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

    void reset ()
    {
        _pbuf->clear();
        _off = 0;
    }

    char const * data () const noexcept
    {
        return _pbuf->data();
    }

    std::size_t size () const noexcept
    {
        return _pbuf->size();
    }

    std::vector<char> take ()
    {
        // Only owned vector can be moved
        if (!_has_ownership)
            throw error { std::make_error_code(std::errc::operation_not_permitted) };

        auto result = std::move(*_pbuf);
        return result;
    }

    template <typename T>
    binary_ostream & operator << (T const & v)
    {
        pack(*this, v);
        return *this;
    }

    binary_ostream & operator << (exclude_size)
    {
        _exclude_size = true;
        return *this;
    }

private:
    template <typename T>
    friend typename std::enable_if<std::is_integral<T>::value, void>::type
    pack (binary_ostream & out, T v)
    {
        T a = Endianess == endian::network ? to_network_order(v) : v;
        union u { T v; char b[sizeof(T)]; } d;
        d.v = a;

        out._pbuf->resize(out._pbuf->size() + sizeof(T));
        std::memcpy(out._pbuf->data() + out._off, d.b, sizeof(T));
        out._off += sizeof(T);
    }

    template <typename T>
    friend typename std::enable_if<std::is_enum<T>::value, void>::type
    pack (binary_ostream & out, T v)
    {
        pack(out, static_cast<typename std::underlying_type<T>::type>(v));
    }

    friend void pack (binary_ostream & out, float v)
    {
        union { float f; std::uint32_t d; } x;
        x.f = v;
        pack(out, x.d);

        // static constexpr auto multiplier = (std::numeric_limits<std::int32_t>::max)();
        // int exp = 0;
        // auto mant = static_cast<std::int32_t>(std::frexp(v, & exp) * multiplier);
        // this->operator << (exp);
        // this->operator << (mant);
    }

    friend void pack (binary_ostream & out, double v)
    {
        union { double f; std::uint64_t d; } x;
        x.f = v;
        pack(out, x.d);

        //static constexpr auto multiplier = (std::numeric_limits<std::int64_t>::max)();
        //int exp = 0;
        //auto mant = static_cast<std::int64_t>(std::frexp(v, & exp) * multiplier);
        //this->operator << (exp);
        //this->operator << (mant);
    }

    /**
     * Writes raw sequence into the stream without size.
     *
     * @throw error {std::errc::result_out_of_range} if @a raw points to reverse
     *        sequence.
     */
    friend void pack (binary_ostream & out, std::pair<char const *, char const *> const & raw)
    {
        if (raw.first > raw.second)
            throw error { std::make_error_code(std::errc::result_out_of_range) };

        auto nbytes = static_cast<size_type>(raw.second - raw.first);

        if (nbytes > 0) {
            if (!out._exclude_size)
                pack(out, nbytes);

            out._pbuf->resize(out._pbuf->size() + nbytes);
            std::memcpy(out._pbuf->data() + out._off, raw.first, nbytes);
            out._off += nbytes;
        } else {
            if (!out._exclude_size)
                pack(out, nbytes);
        }

        out._exclude_size = false;
    }

    friend void pack (binary_ostream & out, std::pair<char const *, size_type> const & raw)
    {
        pack(out, std::make_pair(raw.first, raw.first + raw.second));
    }

    /**
     * @throw error {std::errc::value_too_large} if @a sw is too long.
     */
    friend void pack (binary_ostream & out, string_view sw)
    {
        if (sw.size() > 0) {
            if ((sw.size() > (std::numeric_limits<size_type>::max)()))
                throw error { std::make_error_code(std::errc::value_too_large) };

            if (!out._exclude_size)
                pack(out, static_cast<size_type>(sw.size()));

            out._pbuf->resize(out._pbuf->size() + sw.size());
            std::memcpy(out._pbuf->data() + out._off, sw.data(), sw.size());
            out._off += numeric_cast<size_type>(sw.size());
        } else {
            if (!out._exclude_size)
                pack(out, numeric_cast<size_type>(sw.size()));
        }

        out._exclude_size = false;
    }

    friend void pack (binary_ostream & out, char const * s)
    {
        pack(out, string_view(s, std::strlen(s)));
    }

    friend void pack (binary_ostream & out, std::string const & s)
    {
        pack(out, string_view{s});
    }

    friend void pack (binary_ostream & out, std::vector<char> const & s)
    {
        pack(out, string_view{s.data(), s.size()});
    }

    friend void pack (binary_ostream & out, std::vector<std::uint8_t> const & s)
    {
        pack(out, string_view{reinterpret_cast<char const *>(s.data()), s.size()});
    }

    template <std::size_t N>
    friend void pack (binary_ostream & out, std::array<char, N> const & a)
    {
        pack(out, string_view{a.data(), a.size()});
    }

    template <std::size_t N>
    friend void pack (binary_ostream & out, std::array<std::uint8_t, N> const & a)
    {
        pack(out, string_view{reinterpret_cast<char const *>(a.data()), a.size()});
    }
};

} // namespace pfs
