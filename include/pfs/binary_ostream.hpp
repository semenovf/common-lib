////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2023-2025 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2023.03.22 Initial version.
//      2025.01.23 Refactored.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "error.hpp"
#include "endian.hpp"
#include "namespace.hpp"
#include "numeric_cast.hpp"
#include "string_view.hpp"
#include <cmath>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>

PFS__NAMESPACE_BEGIN

// struct reserve_space { std::size_t sz; };

template <endian Endianess = endian::native>
class binary_ostream
{
public:
    using archive_type = std::vector<char>;
    using size_type = std::uint32_t;
    using offset_type = size_type;

private:
    archive_type * _pbuf {nullptr};
    offset_type _off {0};
    bool _has_ownership {false};

public:
    binary_ostream (size_type reserve_bytes = 0)
        : _has_ownership(true)
    {
        _pbuf = new archive_type;

        if (reserve_bytes > 0) {
            _pbuf->reserve(reserve_bytes);
        }
    }

    binary_ostream (archive_type & buf, offset_type offset = 0)
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

    archive_type take ()
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

private:
    template <typename T>
    friend typename std::enable_if<std::is_same<typename std::decay<T>::type, bool>::value, void>::type
    pack (binary_ostream & out, T v)
    {
        union u { std::int8_t v; char b[sizeof(std::int8_t)]; } d;
        d.v = static_cast<std::int8_t>(v);

        out._pbuf->resize(out._pbuf->size() + sizeof(std::int8_t));
        std::memcpy(out._pbuf->data() + out._off, d.b, sizeof(std::int8_t));
        out._off += sizeof(std::int8_t);
    }

    template <typename T>
    friend typename std::enable_if<std::is_integral<T>::value && !std::is_same<typename std::decay<T>::type, bool>::value, void>::type
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
     * Writes raw sequence into the stream.
     */
    friend void pack (binary_ostream & out, char const * s, std::size_t n)
    {
        if (n == 0)
            return;

        out._pbuf->resize(out._pbuf->size() + n);
        std::memcpy(out._pbuf->data() + out._off, s, n);
        out._off += n;
    }

    friend void pack (binary_ostream & out, char const * s)
    {
        pack(out, s, std::strlen(s));
    }

    friend void pack (binary_ostream & out, pfs::string_view const & s)
    {
        pack(out, s, s.size());
    }

    friend void pack (binary_ostream & out, std::string const & s)
    {
        pack(out, s.data(), s.size());
    }

    friend void pack (binary_ostream & out, std::vector<char> const & s)
    {
        pack(out, s.data(), s.size());
    }

    friend void pack (binary_ostream & out, std::vector<std::uint8_t> const & s)
    {
        pack(out, reinterpret_cast<char const *>(s.data()), s.size());
    }

    template <std::size_t N>
    friend void pack (binary_ostream & out, std::array<char, N> const & a)
    {
        pack(out, a.data(), a.size());
    }

    template <typename SizeType>
    friend void pack (binary_ostream & out, std::pair<char const *, SizeType> const & v)
    {
        pack(out, v.second);
        pack(out, v.first, pfs::numeric_cast<std::size_t>(v.second));
    }

    template <typename SizeType>
    friend void pack (binary_ostream & out, std::pair<char *, SizeType> const & v)
    {
        pack(out, v.second);
        pack(out, v.first, pfs::numeric_cast<std::size_t>(v.second));
    }
};

PFS__NAMESPACE_END
