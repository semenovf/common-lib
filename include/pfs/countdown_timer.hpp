////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2025.01.23 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "namespace.hpp"
#include <chrono>

PFS__NAMESPACE_BEGIN

template<typename period = std::micro, typename rep = std::int64_t>
class countdown_timer
{
public:
    using duration_type = std::chrono::duration<rep, period>;

private:
    duration_type _limit {0};
    std::chrono::time_point<std::chrono::high_resolution_clock> _begin;

public:
    countdown_timer (duration_type limit)
        : _limit(limit)
    {
        reset();
    }

    inline void reset ()
    {
        _begin = std::chrono::high_resolution_clock::now();
    }

    inline duration_type remain () const
    {
        using namespace std::chrono;
        auto diff = std::chrono::high_resolution_clock::now() - _begin;
        return _limit > diff ? duration_cast<duration_type>(_limit - diff) : duration_type{0};
    }

    inline rep remain_count () const
    {
        return remain().count();
    }
};

PFS__NAMESPACE_END
