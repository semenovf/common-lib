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
#include <vector>

namespace pfs {

template <endian Endian = endian::native>
class binary_ostream
{
    std::vector<char> * _pbuf {nullptr};
    std::vector<char>::size_type _off {0};
    bool _has_ownership {false};

public:
    binary_ostream (std::size_t reserve_bytes = 0)
        : _has_ownership(true)
    {
        _pbuf = new std::vector<char>;

        if (reserve_bytes > 0) {
            _pbuf->reserve(reserve_bytes);
        }
    }

    binary_ostream (std::vector<char> & buf, std::vector<char>::size_type offset = 0)
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
     * @throw error {std::errc::value_too_large} if @a sw is too big.
     */
    binary_ostream & operator << (string_view sw)
    {
        if ((sw.size() > (std::numeric_limits<std::int32_t>::max)())) {
            throw error { std::make_error_code(std::errc::value_too_large) };
        }

        this->operator << (static_cast<std::int32_t>((sw.size())));

        _pbuf->resize(_pbuf->size() + sw.size());
        std::memcpy(_pbuf->data() + _off, sw.data(), sw.size());
        _off += sw.size();
        return *this;
    }

    binary_ostream & operator << (char const * s)
    {
        return this->operator << (string_view{s, std::strlen(s)});
    }

    binary_ostream & operator << (std::string const & s)
    {
        return this->operator << (string_view{s});
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
