////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2025.01.25 Initial version.
//      2025.03.25 Moved from `netty-lib`.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "namespace.hpp"
#include "optional.hpp"
#include "universal_id.hpp"
#include "universal_id_rep.hpp"
#include <string>

PFS__NAMESPACE_BEGIN

struct universal_id_traits
{
    using type = pfs::universal_id;
    using rep_type = pfs::universal_id_rep;

    static std::string to_string (type const & id)
    {
        return pfs::to_string(id);
    }

    static std::string to_string (rep_type const & rep)
    {
        return pfs::to_string(cast(rep));
    }

    static type cast (rep_type const & rep)
    {
        return pfs::make_uuid(rep.h, rep.l);
    }

    static rep_type cast (type const & id)
    {
        return rep_type {high(id), low(id)};
    }

    static pfs::optional<type> parse (std::string const & s)
    {
        return parse_universal_id(s);
    }
};

PFS__NAMESPACE_END
