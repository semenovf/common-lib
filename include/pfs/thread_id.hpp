////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2025.12.05 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "namespace.hpp"
#include "fmt.hpp"
#include <sstream>
#include <thread>

PFS__NAMESPACE_BEGIN

inline std::string to_string (std::thread::id const & tid)
{
    std::ostringstream out;
    out << tid;
    return out.str();
}

PFS__NAMESPACE_END

namespace fmt {

    template <>
    struct formatter<std::thread::id>
    {
        template <typename ParseContext>
        constexpr auto parse (ParseContext & ctx) const -> decltype(ctx.begin())
        {
            return ctx.begin();
        }

        template <typename FormatContext>
        auto format (std::thread::id const & tid, FormatContext & ctx) const -> decltype(ctx.out())
        {
            return format_to(ctx.out(), "{}", pfs::to_string(tid));
        }
    };

} // namespace fmt
