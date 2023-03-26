////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020-2023 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2023.03.26 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once

namespace pfs {

class null_mutex
{
public:
    void lock () {}
    bool try_lock () { return true; }
    void unlock () {}
};

} // namespace pfs

