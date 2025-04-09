////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2025.04.09 Initial version
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "namespace.hpp"

PFS__NAMESPACE_BEGIN

class fake_mutex
{
public:
    void lock () {}
    void unlock () {}
    bool try_lock () {return true;}
};

PFS__NAMESPACE_END
