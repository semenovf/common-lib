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

PFS__NAMESPACE_BEGIN

namespace v2 {

template <typename Archive = std::vector<char>, endian Endianess = endian::native>
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
        pack(v);
        return *this;
    }

private:
    template <typename T>
    typename std::enable_if<
        std::is_integral<typename std::decay<T>::type>::value
            || std::is_floating_point<typename std::decay<T>::type>::value, void>::type
    pack (T const & v)
    {
        _ar->resize(_ar->size() + sizeof(T));
        pack_unsafe<Endianess>(_ar->data() + _off, v);
        _off += sizeof(T);
    }

    template <typename T>
    typename std::enable_if<std::is_enum<T>::value, void>::type
    pack (T const & v)
    {
        pack(static_cast<typename std::underlying_type<T>::type>(v));
    }

    /**
     * Writes raw sequence into the stream.
     */
    void pack (char const * s, std::size_t n)
    {
        if (n == 0)
            return;

        _ar->resize(_ar->size() + n);
        std::copy(s, s + n, _ar->data() + _off);
        _off += n;
    }

    void pack (char const * s)
    {
        pack(s, std::strlen(s));
    }

    void pack (std::string const & s)
    {
        pack(s.data(), s.size());
    }

    void pack (pfs::string_view const & s)
    {
        pack(s.data(), s.size());
    }

    void pack (std::vector<char> const & s)
    {
        pack(s.data(), s.size());
    }

    void pack (std::vector<std::uint8_t> const & s)
    {
        pack(reinterpret_cast<char const *>(s.data()), s.size());
    }

    template <std::size_t N>
    void pack (std::array<char, N> const & a)
    {
        pack(a.data(), a.size());
    }
};

} // namespace v2

PFS__NAMESPACE_END
