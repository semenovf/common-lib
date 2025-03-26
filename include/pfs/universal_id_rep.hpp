////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2025.03.25 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "endian.hpp"
#include "binary_istream.hpp"
#include "binary_ostream.hpp"
#include "namespace.hpp"
#include <cstdint>
#include <functional>

PFS__NAMESPACE_BEGIN

class universal_id_rep
{
public:
    std::uint64_t h {0}; // High part of the node
    std::uint64_t l {0}; // Low part of the node

public:
    constexpr universal_id_rep (std::uint64_t h, std::uint64_t l): h(h), l(l) {}

    universal_id_rep () = default;
    universal_id_rep (universal_id_rep const &) = default;
    universal_id_rep (universal_id_rep &&) = default;
    universal_id_rep & operator = (universal_id_rep const &) = default;
    universal_id_rep & operator = (universal_id_rep &&) = default;
    ~universal_id_rep () = default;

public:
    constexpr bool operator == (universal_id_rep const & other) const noexcept
    {
        return h == other.h && l == other.l;
    }

    constexpr bool operator != (universal_id_rep const & other) const noexcept
    {
        return h != other.h || l != other.l;
    }

    constexpr bool operator < (universal_id_rep const & other) const noexcept
    {
        return h < other.h || (!(other.h < h) && l < other.l);
    }

    constexpr bool operator <= (universal_id_rep const & other) const noexcept
    {
        return !(other < *this);
    }

    constexpr bool operator > (universal_id_rep const & other) const noexcept
    {
        return other < *this;
    }

    constexpr bool operator >= (universal_id_rep const & other) const noexcept
    {
        return !(*this < other);
    }
};

template <endian Endianess>
void pack (binary_ostream<Endianess> & out, universal_id_rep const & uuid)
{
    out << uuid.h << uuid.l;
}

template <endian Endianess>
void unpack (binary_istream<Endianess> & in, universal_id_rep & uuid)
{
    in >> uuid.h >> uuid.l;
}

PFS__NAMESPACE_END

namespace std {

template<>
struct hash<pfs::universal_id_rep>
{
    // See https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
    std::size_t operator () (pfs::universal_id_rep const & u) const noexcept
    {
        std::hash<std::uint64_t> hasher;
        auto result = hasher(u.h);
        result ^= hasher(u.l) + 0x9e3779b9 + (result << 6) + (result >> 2);
        return result;
    }
};

} // namespace std
