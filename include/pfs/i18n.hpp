////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2022 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2022.06.10 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "fmt.hpp"
#include <libintl.h>

//
// `xgettext` can recognizes keywords in format `NS::keyword` ignoring namespace
// name `NS`. So this feature can be used in the way implemented in this demo.
//
namespace tr {

struct plural
{
    char const * msgid;
    char const * msgid_plural;
    unsigned long int n;

    plural (char const * m, char const * p, unsigned long int k)
        : msgid(m), msgid_plural(p), n(k)
    {}

    plural (std::string & m, std::string & p, unsigned long int k)
        : msgid(m.c_str()), msgid_plural(p.c_str()), n(k)
    {}
};

struct domain_category
{
    char const * domainname;
    int category;

    domain_category (char const * dname = nullptr, int c = -1)
        : domainname(dname), category(c)
    {}

    domain_category (std::string & dname, int c = -1)
        : domainname(dname.empty() ? nullptr : dname.c_str()), category(c)
    {}
};

inline char * _ (char const * msgid)
{
    return gettext(msgid);
}

inline char * _ (std::string & msgid)
{
    return gettext(msgid.c_str());
}

inline char * _ (char const * msgid, domain_category const & dc)
{
    return (dc.domainname && dc.category > 0)
        ? dcgettext(dc.domainname, msgid, dc.category)
        : dc.domainname
            ? dgettext(dc.domainname, msgid)
            : gettext(msgid);
}

inline char * _ (std::string & msgid, domain_category const & dc)
{
    return _(msgid.c_str(), dc);
}

inline char * n_ (plural const & p)
{
    return ngettext(p.msgid, p.msgid_plural, p.n);
}

inline char * n_ (plural const & p, domain_category const & dc)
{
    return (dc.domainname && dc.category > 0)
        ? dcngettext(dc.domainname, p.msgid, p.msgid_plural, p.n, dc.category)
        : dc.domainname
            ? dngettext(dc.domainname, p.msgid, p.msgid_plural, p.n)
            : ngettext(p.msgid, p.msgid_plural, p.n);
}

template <typename... T>
inline std::string f_ (char const * msgid, T &&... args)
{
    return fmt::format(gettext(msgid), std::forward<T>(args)...);
}

template <typename... T>
inline std::string f_ (std::string & msgid, T &&... args)
{
    return fmt::format(gettext(msgid.c_str()), std::forward<T>(args)...);
}

template <typename... T>
inline std::string f_ (char const * msgid, domain_category const & dc, T &&... args)
{
    return fmt::format((dc.domainname && dc.category > 0)
        ? dcgettext(dc.domainname, msgid, dc.category)
        : dc.domainname
            ? dgettext(dc.domainname, msgid)
            : gettext(msgid)
        , std::forward<T>(args)...);
}

constexpr char const * noop_ (char const * msgid)
{
    return msgid;
}

} // namespace tr
