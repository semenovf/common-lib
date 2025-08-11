////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2025.08.06 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../error.hpp"
#include "../endian.hpp"
#include "../namespace.hpp"
#include "../string_view.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <type_traits>
#include <vector>
#include <utility>

PFS__NAMESPACE_BEGIN

namespace v2 {

template <endian Endianess = endian::native, typename Archive = std::vector<char>>
class binary_ostream
{
public:
    using archive_type = Archive;

private:
    archive_type * _ar {nullptr};
    std::size_t _off {0};

public:
    explicit binary_ostream (archive_type & ar, std::size_t offset = 0) noexcept
        : _ar(& ar)
        , _off(offset)
    {}

    binary_ostream (binary_ostream && other) noexcept
        : _ar(other._ar)
        , _off(other._off)
    {
        other._ar = nullptr;
        other._off = 0;
    }

    binary_ostream & operator = (binary_ostream && other) noexcept
    {
        if (this != & other) {
            using std::swap;

            binary_ostream tmp {std::move(other)};
            std::swap(_ar, tmp._ar);
            std::swap(_off, tmp._off);
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

private:
    template <typename T>
    friend typename std::enable_if<std::is_arithmetic<typename std::decay<T>::type>::value, void>::type
    pack (binary_ostream & out, T const & v)
    {
        out._ar->resize(out._ar->size() + sizeof(T));
        pack_unsafe<Endianess>(out._ar->data() + out._off, v);
        out._off += sizeof(T);
    }

    template <typename T>
    friend typename std::enable_if<std::is_enum<T>::value, void>::type
    pack (binary_ostream & out, T const & v)
    {
        pack(out, static_cast<typename std::underlying_type<T>::type>(v));
    }

    /**
     * Writes raw sequence into the stream.
     */
    friend void pack (binary_ostream & out, char const * s, std::size_t n)
    {
        if (n == 0)
            return;

        out._ar->resize(out._ar->size() + n);
        std::copy(s, s + n, out._ar->data() + out._off);
        out._off += n;
    }

    friend void pack (binary_ostream & out, std::pair<char const *, std::size_t> const & v)
    {
        pack(out, v.first, v.second);
    }

    friend void pack (binary_ostream & out, char const * s)
    {
        pack(out, s, std::strlen(s));
    }

    friend void pack (binary_ostream & out, std::string const & s)
    {
        pack(out, s.data(), s.size());
    }

    friend void pack (binary_ostream & out, pfs::string_view const & s)
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
};

} // namespace v2

PFS__NAMESPACE_END
