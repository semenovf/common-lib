////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2023-2025 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2023.03.22 Initial version.
//      2025.01.23 Refactored.
//      2025.08.12 Moved from `v2` namespace.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "error.hpp"
#include "endian.hpp"
#include "namespace.hpp"
#include "string_view.hpp"
#include <array>
#include <cstdint>
#include <type_traits>
#include <vector>
#include <utility>

PFS__NAMESPACE_BEGIN

template <endian Endianess = endian::native, typename Archive = std::vector<char>>
class binary_ostream
{
public:
    using archive_type = Archive;

private:
    archive_type * _ar {nullptr};

public:
    explicit binary_ostream (archive_type & ar) noexcept
        : _ar(& ar)
    {}

    binary_ostream (binary_ostream && other) noexcept
        : _ar(other._ar)
    {
        other._ar = nullptr;
    }

    binary_ostream & operator = (binary_ostream && other) noexcept
    {
        if (this != & other) {
            using std::swap;

            binary_ostream tmp {std::move(other)};
            std::swap(_ar, tmp._ar);
        }

        return *this;
    }

    binary_ostream (binary_ostream const &) = delete;
    binary_ostream & operator = (binary_ostream const &) = delete;

    ~binary_ostream ()
    {
        _ar = nullptr;
    }

    template <typename T>
    binary_ostream & operator << (T const & v)
    {
        pack(*this, v);
        return *this;
    }

    /**
     * Writes raw sequence into the stream.
     */
    binary_ostream & write (char const * s, std::size_t n)
    {
        if (n > 0)
            write(*_ar, s, n);

        return *this;
    }

private:
    /**
     * Required implementation for the specified Archive class.
     */
    void write (archive_type & ar, char const * s, std::size_t n);

    template <typename T>
    friend typename std::enable_if<std::is_integral<typename std::decay<T>::type>::value, void>::type
    pack (binary_ostream & out, T const & v)
    {
        T x = Endianess == endian::network ? to_network_order(v) : v;
        union { T a; char b[sizeof(T)]; } u;
        u.a = x;
        out.write(u.b, sizeof(T));
    }

    friend void pack (binary_ostream & out, float const & v)
    {
        static_assert(sizeof(float) == sizeof(std::uint32_t)
            , "Float and std::uint32_t are expected to have the same size.");
        union { float f; std::uint32_t d; } u;
        u.f = v;
        pack(out, u.d);
    }

    friend void pack (binary_ostream & out, double const & v)
    {
        static_assert(sizeof(double) == sizeof(std::uint64_t)
            , "Double and std::uint64_t are expected to have the same size.");
        union { double f; std::uint64_t d; } u;
        u.f = v;
        pack(out, u.d);
    }

    template <typename T>
    friend typename std::enable_if<std::is_enum<T>::value, void>::type
    pack (binary_ostream & out, T const & v)
    {
        pack(out, static_cast<typename std::underlying_type<T>::type>(v));
    }

    friend void pack (binary_ostream & out, std::pair<char const *, std::size_t> const & v)
    {
        out.write(v.first, v.second);
    }

    friend void pack (binary_ostream & out, char const * s)
    {
        out.write(s, std::strlen(s));
    }

    friend void pack (binary_ostream & out, std::string const & s)
    {
        out.write(s.data(), s.size());
    }

    friend void pack (binary_ostream & out, pfs::string_view const & s)
    {
        out.write(s.data(), s.size());
    }

    template <std::size_t N>
    friend void pack (binary_ostream & out, std::array<char, N> const & a)
    {
        out.write(a.data(), a.size());
    }
};

template <>
inline void binary_ostream<endian::little, std::vector<char>>::write (std::vector<char> & ar
    , char const * s, std::size_t n)
{
    ar.insert(ar.end(), s, s + n);
}

template <>
inline void binary_ostream<endian::big, std::vector<char>>::write (std::vector<char> & ar
    , char const * s, std::size_t n)
{
    ar.insert(ar.end(), s, s + n);
}

template <>
inline void binary_ostream<endian::little, std::vector<unsigned char>>::write (std::vector<unsigned char> & ar
    , char const * s, std::size_t n)
{
    ar.insert(ar.end(), reinterpret_cast<unsigned char const *>(s)
        , reinterpret_cast<unsigned char const *>(s + n));
}

template <>
inline void binary_ostream<endian::big, std::vector<unsigned char>>::write (std::vector<unsigned char> & ar
    , char const * s, std::size_t n)
{
    ar.insert(ar.end(), reinterpret_cast<unsigned char const *>(s)
        , reinterpret_cast<unsigned char const *>(s + n));
}

PFS__NAMESPACE_END
