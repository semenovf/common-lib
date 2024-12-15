////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2024 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2024.12.15 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "namespace.hpp"

PFS__NAMESPACE_BEGIN

template <typename F>
class scope_exit 
{
    bool _released {false};
    F _on_exit;

public:
    scope_exit (F && f) : _on_exit(std::move(f)) {}

    scope_exit (scope_exit && other) noexcept:
        : _released(other._released)
        , _on_exit(std::move(other._on_exit))
    {
        other._released = true;
    }

    ~scope_exit ()  
    { 
        if (!_released) 
            _on_exit();
    }

    void release () 
    { 
        _released = true; 
    }
};

template <typename F>
scope_exit<F> make_scope_exit (F && f)
{
    return scope_exit<F>(std::move(f));
}

PFS__NAMESPACE_END
