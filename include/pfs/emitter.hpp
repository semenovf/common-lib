////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Vladislav Trifochkin
//
// This file is part of [pfs-common](https://github.com/semenovf/pfs-common) library.
//
// Changelog:
//      2019.12.19 Initial version (inhereted from https://github.com/semenovf/pfs)
//      2021.04.25 Moved from pfs-modulus into pfs-common
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "type_traits.hpp"
#include "function_queue.hpp"
#include <functional>
#include <list>
#include <mutex>
#include <cassert>

namespace pfs {

template <typename ...Args>
class emitter
{
    using detector_type = std::function<void(Args...)>;

    // Detector list type requirements:
    //      * no iterators are invalidated while inserting and emplacing
    //      * no iterators are invalidated while erasing (excluding erased)
    using detector_list = std::list<detector_type>;
    using detector_iterator = typename detector_list::iterator;

    detector_list _detectors;

public:
    using iterator = detector_iterator;

public:
    /**
     * Connect detector defined as ordinary function
     */

    template <typename F
        , typename = typename std::enable_if<std::is_same<void (*) (Args...), F>::value, F>::type>
    iterator connect (F f)
    {
        _detectors.emplace_back(f);
        return --_detectors.end();
    }

    iterator connect (std::function<void(Args...)> && f)
    {
        _detectors.emplace_back(f);
        return --_detectors.end();
    }

    /**
     * Connect detector defined as member function
     */
    template <typename Class>
    iterator connect (Class & c, void (Class::*f) (Args...))
    {
        _detectors.emplace_back([& c, f] (Args... args) { (c.*f)(args...); });
        return --_detectors.end();
    }

    template <template <typename> class QueueContainer, size_t capacity_increment>
    iterator connect (function_queue<QueueContainer, capacity_increment> & q
        , std::function<void(Args...)> && f)
    {
        _detectors.emplace_back([& q, f] (Args... args) {
            q.push(std::move(f), args...);
        });

        return --_detectors.end();
    }

    template <template <typename> class QueueContainer, size_t capacity_increment, typename Class>
    iterator connect (function_queue<QueueContainer, capacity_increment> & q
        , Class & c, void (Class::*f) (Args...))
    {
        _detectors.emplace_back([& q, & c, f] (Args... args) {
            q.push(f, & c, args...);
        });

        return --_detectors.end();
    }

    /**
     * Disconnect detector specified by position @a pos (previously returned
     * by connect() call)
     */
    void disconnect (iterator pos)
    {
        _detectors.erase(pos);
    }

    /**
     * Disconnect all detectors connected to this emitter
     */
    void disconnect_all ()
    {
        _detectors.clear();
    }

    void operator () (Args... args)
    {
        for (auto & f: _detectors) {
            f(args...);
        }
    }
};

////////////////////////////////////////////////////////////////////////////////
// emitter_mt
////////////////////////////////////////////////////////////////////////////////
template <typename ...Args>
class emitter_mt: protected emitter<Args...>
{
    using base_class = emitter<Args...>;
    using mutex_type = std::mutex;

    mutex_type _mtx;

public:
    using iterator = typename base_class::iterator;

public:
    // Apply base class constructors
    using base_class::base_class;

    template <typename F
        , typename = typename std::enable_if<std::is_same<void (*) (Args...), F>::value, F>::type>
    iterator connect (F f)
    {
        std::unique_lock<mutex_type> locker{_mtx};
        return base_class::template connect<F>(f);
    }

    iterator connect (std::function<void(Args...)> && f)
    {
        std::unique_lock<mutex_type> locker{_mtx};
        return base_class::connect(std::forward<std::function<void(Args...)>>(f));
    }

    template <typename Class>
    iterator connect (Class & c, void (Class::*f) (Args...))
    {
        std::unique_lock<mutex_type> locker{_mtx};
        return base_class::template connect<Class>(c, f);
    }


    template <template <typename> class QueueContainer, size_t capacity_increment>
    iterator connect (function_queue<QueueContainer, capacity_increment> & q
        , std::function<void(Args...)> && f)
    {
        std::unique_lock<mutex_type> locker{_mtx};
        return base_class::template connect<QueueContainer, capacity_increment>(q, f);
    }

    template <template <typename> class QueueContainer, size_t capacity_increment, typename Class>
    iterator connect (function_queue<QueueContainer, capacity_increment> & q
        , Class & c, void (Class::*f) (Args...))
    {
        std::unique_lock<mutex_type> locker{_mtx};
        return base_class::template connect<QueueContainer, capacity_increment, Class>(q, c, f);
    }

    /**
     * Disconnect detector specified by position @a pos (previously returned
     * by connect() call)
     */
    void disconnect (iterator pos)
    {
        std::unique_lock<mutex_type> locker{_mtx};
        base_class::disconnect(pos);
    }

    /**
     * Disconnect all detectors connected to this emitter
     */
    void disconnect_all ()
    {
        std::unique_lock<mutex_type> locker{_mtx};
        base_class::disconnect_all();
    }

    void operator () (Args... args)
    {
        std::unique_lock<mutex_type> locker{_mtx};
        base_class::operator()(args...);
    }
};

} // pfs
