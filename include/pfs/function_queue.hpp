////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2019.12.19 Initial version (inhereted from https://github.com/semenovf/pfs)
//      2020.10.26 Changed default_queue_container (ring_buffer_mt now)
//      2021.05.07 Moved from pfs-modulus into common-lib
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "ring_buffer.hpp"
#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <utility>
#include <cassert>

namespace pfs {

namespace details {

template <typename T>
using function_queue_container = ring_buffer_mt<T, 256>;

} // namespace details

template <typename F, typename... Args>
inline auto active_bind (F && func, Args &&... args)
    -> decltype(std::bind(std::forward<F>(func), std::forward<Args>(args)...))
{
    return std::bind(std::forward<F>(func), std::forward<Args>(args)...);
}

template <template <typename> class QueueContainer = details::function_queue_container
    , size_t capacity_increment = 256>
class function_queue
{
public:
    using value_type = std::function<void ()>;
    using queue_container_type = QueueContainer<value_type>;
    using size_type = typename queue_container_type::size_type;

private:
    queue_container_type _q;
    size_type _capacity_inc {capacity_increment};

public:
    function_queue (size_type capacity_inc = capacity_increment)
        : _capacity_inc(capacity_inc != 0 ? capacity_inc : capacity_increment)
    {}

    virtual ~function_queue ()
    {
        clear();
    }

    bool empty () const
    {
        return _q.empty();
    }

    /**
     * @return Number of elements ready to call or in process (already calling).
     */
    size_type count () const
    {
        return _q.size();
    }

    size_type size () const
    {
        return _q.size();
    }

    void clear ()
    {
        _q.clear();
    }

    template <typename F, typename ...Args>
    void push (F && f, Args &&... args)
    {
        auto result = _q.try_push(active_bind(std::forward<F>(f), std::forward<Args>(args)...)
            , _capacity_inc);
        assert(result);
    }

    void call ()
    {
        value_type caller;

        if (_q.try_pop(caller)) {
            caller();
        }
    }

    void call (int max_count)
    {
        if (max_count > 0) {
            while (!this->empty() && max_count--)
                call();
        }
    }

    void call_all ()
    {
        while (!this->empty())
            call();
    }

    void wait ()
    {
        _q.wait();
    }

    bool wait_for (intmax_t microseconds)
    {
        using rep_type = std::chrono::microseconds::rep;
        using period_type = std::chrono::microseconds::period;

        return _q.template wait_for<rep_type, period_type>(std::chrono::microseconds(microseconds));
    }
};

} // namespace pfs

