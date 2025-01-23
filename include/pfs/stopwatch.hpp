////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2021.12.06 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <chrono>

namespace pfs {
//
// https://www.reddit.com/r/cpp_questions/comments/40f6xn/timer_using_stdchronosteady_clock/
//
template<typename period = std::micro, typename rep = std::int64_t>
class stopwatch
{
    std::chrono::time_point<std::chrono::high_resolution_clock> _begin, _end;

public:
    using duration_type = std::chrono::duration<rep, period>;

    stopwatch ()
    {
        _begin = std::chrono::high_resolution_clock::now();
        _end = _begin;
    }

    inline void start ()
    {
        _begin = std::chrono::high_resolution_clock::now();
    }

    inline void stop ()
    {
        _end = std::chrono::high_resolution_clock::now();
    }

    inline rep count () const
    {
        using namespace std::chrono;
        return duration_cast<duration_type>(_end - _begin).count();
    }

    inline rep current_count () const
    {
        using namespace std::chrono;
        return duration_cast<duration_type>(std::chrono::high_resolution_clock::now() - _begin).count();
    }
};

} // namespace pfs
