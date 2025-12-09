////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2025.12.09 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "namespace.hpp"
#include <signal.h>

PFS__NAMESPACE_BEGIN

#if _MSC_VER
using sighandler_t = void (*) (int);
#endif

class signal_guard
{
    int sig {0};
    sighandler_t old_handler;

public:
    signal_guard (int sig, sighandler_t handler)
        : sig(sig)
    {
        old_handler = signal(sig, handler);
    }

    ~signal_guard ()
    {
        signal(sig, old_handler);
    }
};

PFS__NAMESPACE_END
