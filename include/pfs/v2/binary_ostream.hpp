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
    using size_type = std::uint32_t;
    using offset_type = size_type;

private:
    archive_type * _ar {nullptr};
    offset_type _off {0};

public:
    binary_ostream (archive_type & ar, offset_type offset = 0)
        : _ar(& ar)
        , _off(offset)
    {}

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
    static typename std::enable_if<
        std::is_integral<typename std::decay<T>::type>::value
            || std::is_floating_point<typename std::decay<T>::type>::value, void>::type
    pack (binary_ostream & out, T const & v)
    {
        out._ar->resize(out._ar->size() + sizeof(T));
        pack_unsafe<Endianess>(out._ar->data() + out._off, v);
        out._off += sizeof(T);
    }

    template <typename T>
    static typename std::enable_if<std::is_enum<T>::value, void>::type
    pack (binary_ostream & out, T const & v)
    {
        pack(out, static_cast<typename std::underlying_type<T>::type>(v));
    }

    /**
     * Writes raw sequence into the stream.
     */
    static void pack (binary_ostream & out, char const * s, std::size_t n)
    {
        if (n == 0)
            return;

        out._ar->resize(out._ar->size() + n);
        std::memcpy(out._ar->data() + out._off, s, n);
        out._off += n;
    }

    static void pack (binary_ostream & out, char const * s)
    {
        pack(out, s, std::strlen(s));
    }

    static void pack (binary_ostream & out, std::string const & s)
    {
        pack(out, s.data(), s.size());
    }

    static void pack (binary_ostream & out, pfs::string_view const & s)
    {
        pack(out, s.data(), s.size());
    }

    static void pack (binary_ostream & out, std::vector<char> const & s)
    {
        pack(out, s.data(), s.size());
    }

    static void pack (binary_ostream & out, std::vector<std::uint8_t> const & s)
    {
        pack(out, reinterpret_cast<char const *>(s.data()), s.size());
    }

    template <std::size_t N>
    static void pack (binary_ostream & out, std::array<char, N> const & a)
    {
        pack(out, a.data(), a.size());
    }
};

} // namespace v2

PFS__NAMESPACE_END
