////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2019.12.19 Initial version (inhereted from https://github.com/semenovf/pfs)
//      2021.04.25 Moved from pfs-modulus into common-lib
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "type_traits.hpp"
#include "function_queue.hpp"
#include <atomic>
#include <functional>
#include <list>
#include <mutex>
#include <thread>
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

    inline std::size_t size () const
    {
        return _detectors.size();
    }

    inline bool has_detectors () const
    {
        return size() > 0;
    }
};

////////////////////////////////////////////////////////////////////////////////
// emitter_mt_basic
////////////////////////////////////////////////////////////////////////////////
template <typename MutexType, typename ...Args>
class emitter_mt_basic: protected emitter<Args...>
{
    using base_class = emitter<Args...>;
    using mutex_type = MutexType;

    mutable mutex_type _mtx;

public:
    using iterator = typename base_class::iterator;

private:
    // https://www.justsoftwaresolutions.co.uk/threading/thread-safe-copy-constructors.html
    emitter_mt_basic (emitter_mt_basic const & other, std::lock_guard<mutex_type> const &)
        : base_class(other)
    {}

    emitter_mt_basic (emitter_mt_basic && other, std::lock_guard<mutex_type> const &)
        : base_class(std::move(other))
    {}

public:
    emitter_mt_basic () = default;
    ~emitter_mt_basic () = default;

    emitter_mt_basic (emitter_mt_basic const & other)
        : emitter_mt_basic(other, std::lock_guard<mutex_type>(other._mtx))
    {}

    // https://stackoverflow.com/questions/4037853/thread-safe-copy-constructor-assignment-operator
    emitter_mt_basic & operator = (emitter_mt_basic const & other)
    {
        if (this != & other) {
            std::unique_lock<mutex_type> this_locker(_mtx, std::defer_lock);
            std::unique_lock<mutex_type> other_locker(other._mtx, std::defer_lock);
            std::lock(this_locker, other_locker);

            base_class::operator = (other);
        }

        return *this;
    }

    emitter_mt_basic (emitter_mt_basic && other)
        : emitter_mt_basic(std::move(other), std::lock_guard<mutex_type>(other._mtx))
    {}

    emitter_mt_basic & operator = (emitter_mt_basic && other)
    {
        if (this != & other) {
            std::unique_lock<mutex_type> this_locker(_mtx, std::defer_lock);
            std::unique_lock<mutex_type> other_locker(other._mtx, std::defer_lock);
            std::lock(this_locker, other_locker);

            base_class::operator = (std::move(other));
        }

        return *this;
    }

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

    inline std::size_t size () const
    {
        std::unique_lock<mutex_type> locker{_mtx};
        return base_class::size();
    }

    inline bool has_detectors () const
    {
        std::unique_lock<mutex_type> locker{_mtx};
        return base_class::has_detectors();
    }
};

#ifdef PFS__TEST_ENABLED
// https://stackoverflow.com/questions/15056237/which-is-more-efficient-basic-mutex-lock-or-atomic-integer
// https://rigtorp.se/spinlock/
struct atomic_mutex
{
    std::atomic<bool> _busy {false};

    inline void lock ()
    {
        while (_busy.exchange(true, std::memory_order_relaxed))
            ;
        std::atomic_thread_fence(std::memory_order_acquire);
    }

    inline void unlock ()
    {
        std::atomic_thread_fence(std::memory_order_release);
        _busy.store(false, std::memory_order_relaxed);
    }

    inline bool try_lock ()
    {
        if (!_busy) {
            lock();
            return true;
        }

        return false;
    }
};

// https://rigtorp.se/spinlock/
struct atomic_mutex_mod
{
    std::atomic<bool> _busy {false};

    void lock ()
    {
        //while (_busy.exchange(true, std::memory_order_acquire));
        for (;;) {
            if (!_busy.exchange(true, std::memory_order_acquire)) {
                break;
            }

            while (_busy.load(std::memory_order_relaxed))
                ;
        }
    }

    void unlock ()
    {
        _busy.store(false, std::memory_order_release);
    }

    inline bool try_lock ()
    {
        if (!_busy) {
            lock();
            return true;
        }

        return false;
    }
};

// https://stackoverflow.com/questions/29193445/implement-a-high-performance-mutex-similar-to-qts-one
// https://gist.github.com/vertextao/9a9077720c15fec89ed1f3fd91c9e91a
class spinlock_mutex
{
    std::atomic_flag _busy = ATOMIC_FLAG_INIT;

public:
    void lock ()
    {
        while (_busy.test_and_set(std::memory_order_acquire))
            std::this_thread::yield(); //<- this is not in the source but might improve performance.
    }

    void unlock ()
    {
        _busy.clear(std::memory_order_release);
    }

    inline bool try_lock ()
    {
        return !_busy.test_and_set(std::memory_order_acquire);
    }
};

// https://stackoverflow.com/questions/29193445/implement-a-high-performance-mutex-similar-to-qts-one
class fast_mutex
{
    // Status of the fast mutex
    std::atomic<bool> _busy {false};

    // helper mutex and vc on which threads can wait in case of collision
    std::mutex _mtx;
    std::condition_variable _cv;

    // the maximum number of threads that might be waiting on the cv (conservative estimation)
    std::atomic<int> _counter {0};

public:
    void lock ()
    {
        if (_busy.exchange(true)) {
            _counter++;

            {
                std::unique_lock<std::mutex> locker(_mtx);
                _cv.wait(locker, [this] { return !_busy.exchange(true); });
            }

            _counter--;
        }
    }

    void unlock ()
    {
        _busy = false;

        if (_counter > 0) {
            std::lock_guard<std::mutex> locker(_mtx);
            _cv.notify_one();
        }
    }

    inline bool try_lock ()
    {
        if (!_busy) {
            lock();
            return true;
        }

        return false;
    }
};

#endif // PFS__TEST_ENABLED

////////////////////////////////////////////////////////////////////////////////
// emitter_mt specializations
////////////////////////////////////////////////////////////////////////////////
template <typename ...Args>
using emitter_mt = emitter_mt_basic<std::mutex, Args...>;

#ifdef PFS__TEST_ENABLED

// Worse than emitter_mt (tested with gcc++-9 on Ubuntu 20.04)
template <typename ...Args>
using emitter_mt_atomic = emitter_mt_basic<atomic_mutex, Args...>;

template <typename ...Args>
using emitter_mt_atomic_mod = emitter_mt_basic<atomic_mutex_mod, Args...>;

// A little bit worse than emitter_mt (tested with gcc++-9 on Ubuntu 20.04)
template <typename ...Args>
using emitter_mt_spinlock = emitter_mt_basic<spinlock_mutex, Args...>;

// Worse than emitter_mt but a little bit better tnan emitter_mt_atomic
// (tested with gcc++-9 on Ubuntu 20.04)
template <typename ...Args>
using emitter_mt_fast = emitter_mt_basic<fast_mutex, Args...>;

#endif // PFS__TEST_ENABLED

} // pfs
