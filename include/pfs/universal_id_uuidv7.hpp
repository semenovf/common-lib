////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2024 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2024.12.23 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "namespace.hpp"
#include "assert.hpp"
#include "fmt.hpp"
#include "optional.hpp"
#include <array>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <random>

#include "log.hpp"

PFS__NAMESPACE_BEGIN

namespace details {
#include "3rdparty/uuidv7/uuidv7.h"
}

// References:
// 1. https://www.rfc-editor.org/rfc/rfc9562
// 2. https://github.com/LiosK/uuidv7-h
// 3. https://antonz.org/uuidv7/#cpp
//
// NOTE! `uuid_t` name conficts with macro `uuid_t` defined in MSVC's shared/rpcdece.h
class universal_id
{
    bool _initialized {false};
    std::array<std::uint8_t, 16> _u;

public:
    universal_id ()
    {
        _u.fill(0);
    }

    universal_id (std::array<std::uint8_t, 16> u)
        : _initialized{true}
        , _u(std::move(u))
    {}

public:
    std::string to_string () const
    {
        char out[37];
        details::uuidv7_to_string(_u.data(), out);
        return std::string(out, out + 36);
    }

    std::array<std::uint8_t, 16> to_array () const
    {
        return _u;
    }

    std::uint64_t high () const noexcept
    {
        std::uint64_t result = static_cast<std::uint64_t>(_u[15])
            | static_cast<std::uint64_t>(_u[14]) << 8
            | static_cast<std::uint64_t>(_u[13]) << 16
            | static_cast<std::uint64_t>(_u[12]) << 24
            | static_cast<std::uint64_t>(_u[11]) << 32
            | static_cast<std::uint64_t>(_u[10]) << 40
            | static_cast<std::uint64_t>(_u[9])  << 48
            | static_cast<std::uint64_t>(_u[8])  << 56;
        return result;
    }

    std::uint64_t low () const noexcept
    {
        std::uint64_t result = static_cast<std::uint64_t>(_u[7])
            | static_cast<std::uint64_t>(_u[6]) << 8
            | static_cast<std::uint64_t>(_u[5]) << 16
            | static_cast<std::uint64_t>(_u[4]) << 24
            | static_cast<std::uint64_t>(_u[3]) << 32
            | static_cast<std::uint64_t>(_u[2]) << 40
            | static_cast<std::uint64_t>(_u[1]) << 48
            | static_cast<std::uint64_t>(_u[0]) << 56;
        return result;
    }

public: // static
    // Used CompareULIDs(const ULID& ulid1, const ULID& ulid2) algorithm
    static int compare (universal_id const & u1, universal_id const & u2)
    {
        if (u1._u[0] != u2._u[0]) {
            return (u1._u[0] < u2._u[0]) * -2 + 1;
        }

        if (u1._u[1] != u2._u[1]) {
            return (u1._u[1] < u2._u[1]) * -2 + 1;
        }

        if (u1._u[2] != u2._u[2]) {
            return (u1._u[2] < u2._u[2]) * -2 + 1;
        }

        if (u1._u[3] != u2._u[3]) {
            return (u1._u[3] < u2._u[3]) * -2 + 1;
        }

        if (u1._u[4] != u2._u[4]) {
            return (u1._u[4] < u2._u[4]) * -2 + 1;
        }

        if (u1._u[5] != u2._u[5]) {
            return (u1._u[5] < u2._u[5]) * -2 + 1;
        }

        if (u1._u[6] != u2._u[6]) {
            return (u1._u[6] < u2._u[6]) * -2 + 1;
        }

        if (u1._u[7] != u2._u[7]) {
            return (u1._u[7] < u2._u[7]) * -2 + 1;
        }

        if (u1._u[8] != u2._u[8]) {
            return (u1._u[8] < u2._u[8]) * -2 + 1;
        }

        if (u1._u[9] != u2._u[9]) {
            return (u1._u[9] < u2._u[9]) * -2 + 1;
        }

        if (u1._u[10] != u2._u[10]) {
            return (u1._u[10] < u2._u[10]) * -2 + 1;
        }

        if (u1._u[11] != u2._u[11]) {
            return (u1._u[11] < u2._u[11]) * -2 + 1;
        }

        if (u1._u[12] != u2._u[12]) {
            return (u1._u[12] < u2._u[12]) * -2 + 1;
        }

        if (u1._u[13] != u2._u[13]) {
            return (u1._u[13] < u2._u[13]) * -2 + 1;
        }

        if (u1._u[14] != u2._u[14]) {
            return (u1._u[14] < u2._u[14]) * -2 + 1;
        }

        if (u1._u[15] != u2._u[15]) {
            return (u1._u[15] < u2._u[15]) * -2 + 1;
        }

        return 0;
    }
};

inline universal_id generate_uuid ()
{
    static std::random_device s_rd; // Will be used to obtain a seed for the random number engine
    static std::mt19937 s_random_engine{s_rd()}; // Standard mersenne_twister_engine seeded with rd()

    // MSVC error C2338: static_assert failed: 'note: char, signed char, unsigned char, char8_t, int8_t, and uint8_t are not allowed'
    //static std::uniform_int_distribution<std::uint8_t> s_distrib{0, 255};
    static std::uniform_int_distribution<std::uint16_t> s_distrib{0, 255};

    std::array<std::uint8_t, 16> u;
    std::uint8_t const * no_uuid_prev = nullptr;
    auto now = std::chrono::system_clock::now();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

    std::array<std::uint8_t, 10> random_bytes;
    std::generate(random_bytes.begin(), random_bytes.end(), [] () { return s_distrib(s_random_engine); });

    details::uuidv7_generate(u.data(), millis, random_bytes.data(), no_uuid_prev);

    return universal_id {std::move(u)};
}

inline std::string to_string (universal_id const & value)
{
    return value.to_string();
}

template <typename T>
T from_string (std::string const & str);

/**
 * @deprecated Use @c parse_universal_id instead
 */
template <>
[[deprecated]] inline universal_id from_string<universal_id> (std::string const & str)
{
    std::array<std::uint8_t, 16> u;

    if (str.front() == '{' && str.back() == '}') {
        if (str.size() != 38)
            return pfs::universal_id{};

        std::array<char, 37> tmp;
        std::copy(str.begin() + 1, str.end(), tmp.begin());
        tmp[36] = '\x0';

        auto rc = details::uuidv7_from_string(tmp.data(), u.data());

        if (rc != 0)
            return pfs::universal_id{};
    } else {
        if (str.size() != 36)
            return pfs::universal_id{};

        auto rc = details::uuidv7_from_string(str.data(), u.data());

        if (rc != 0)
            return pfs::universal_id{};
    }

    return pfs::universal_id {std::move(u)};
}

inline pfs::optional<universal_id> parse_universal_id (char const * s, std::size_t n)
{
    std::array<std::uint8_t, 16> u;

    if (n < 16)
        return pfs::nullopt;

    if (s[0] == '{' && s[n-1] == '}') {
        if (n != 38)
            return pfs::nullopt;

        std::array<char, 37> tmp;
        std::copy(s + 1, s + n - 1, tmp.begin());
        tmp[36] = '\x0';

        auto rc = details::uuidv7_from_string(tmp.data(), u.data());

        if (rc != 0)
            return pfs::nullopt;
    } else {
        if (n != 36)
            return pfs::nullopt;

        auto rc = details::uuidv7_from_string(s, u.data());

        if (rc != 0)
            return pfs::nullopt;
    }

    return pfs::universal_id {std::move(u)};
}

inline pfs::optional<universal_id> parse_universal_id (std::string const & text)
{
    return parse_universal_id(text.data(), text.size());
}

inline int compare (universal_id const & u1, universal_id const & u2)
{
    return universal_id::compare(u1, u2);
}

inline bool operator == (universal_id const & u1, universal_id const & u2)
{
    return compare(u1, u2) == 0;
}

inline bool operator != (universal_id const & u1, universal_id const & u2)
{
    return compare(u1, u2) != 0;
}

inline bool operator < (universal_id const & u1, universal_id const & u2)
{
    return compare(u1, u2) < 0;
}

inline bool operator <= (universal_id const & u1, universal_id const & u2)
{
    return compare(u1, u2) <= 0;
}

inline bool operator > (universal_id const & u1, universal_id const & u2)
{
    return compare(u1, u2) > 0;
}

inline bool operator >= (universal_id const & u1, universal_id const & u2)
{
    return compare(u1, u2) >= 0;
}

PFS__NAMESPACE_END

namespace fmt {

template <>
struct formatter<PFS__NAMESPACE_NAME::universal_id>
{
    template <typename ParseContext>
    constexpr auto parse (ParseContext & ctx) const -> decltype(ctx.begin())
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format (PFS__NAMESPACE_NAME::universal_id const & uuid, FormatContext & ctx) const -> decltype(ctx.out())
    {
        return format_to(ctx.out(), "{{{}}}", to_string(uuid));
    }
};

} // namespace fmt

inline pfs::universal_id operator ""_uuidv7 (char const * str, std::size_t len)
{
    PFS__ASSERT(len == 36 || len == 0, "UUIDv7 literal must be 36 characters or empty");

    if (len == 0)
        return pfs::universal_id{};

    std::array<std::uint8_t, 16> u;
    auto rc = PFS__NAMESPACE_NAME::details::uuidv7_from_string(str, u.data());

    if (rc != 0)
        return pfs::universal_id{};

    return pfs::universal_id {std::move(u)};
}
