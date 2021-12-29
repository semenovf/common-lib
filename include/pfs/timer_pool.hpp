////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020-2021 Vladislav Trifochkin
//
// License: see LICENSE file
//
// This file is part of `common-lib`.
//
// This code inspired from:
//      * https://codereview.stackexchange.com/questions/127552/portable-periodic-one-shot-timer-thread-follow-up
//      * https://github.com/stella-emu/stella/blob/master/src/common/timer_pool.hxx
//      * https://github.com/stella-emu/stella/blob/master/src/common/timer_pool.cxx
// and adapted to requirements of `modulus` and `modulus2-lib` projects.
//
// Changelog:
//      2020.01.14 Initial version.
//      2021.06.29 Added to modulus2-lib and renamed to timer_pool.hpp.
//      2021.09.27 Moved to common-lib from modulus2-lib.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <set>
#include <thread>
#include <unordered_map>
#include <cassert>
#include <cstdint>

namespace pfs {

/**
 * Class timer pool based on code by Doug Gale (doug65536) with modifications
 * and cleanup by Stephen Anthony (appropriate references see above)
 */
class timer_pool
{
public:
    using timer_id = uint32_t;

    // Function object we actually use
    using callback_type = std::function<void()>;

private:
    using mutex_type = std::mutex;
    using condition_variable_type = std::condition_variable;
    using locker_type = std::unique_lock<mutex_type>;
    using clock_type = std::chrono::steady_clock;
    using time_point_type = std::chrono::time_point<clock_type>;
    using duration_millis_type = std::chrono::milliseconds;

    struct timer_item
    {
        timer_id id {0};
        time_point_type next;
        duration_millis_type period;
        callback_type callback;
        bool running {false};

        // You must be holding the 'sync' lock to assign wait_cv
        std::unique_ptr<condition_variable_type> wait_cv;

        explicit timer_item (timer_id tid = 0) : id(tid) { }

        timer_item (timer_item && r) noexcept
            : id(r.id)
            , next(r.next)
            , period(r.period)
            , callback(std::move(r.callback))
            , running(r.running)
        {}

        timer_item & operator = (timer_item && r) noexcept;

        timer_item (timer_id tid
                , time_point_type tnext
                , duration_millis_type tperiod
                , callback_type && func) noexcept
            : id(tid)
            , next(tnext)
            , period(tperiod)
            , callback(std::move(func))
        {}

        timer_item (timer_item const &) = delete;
        timer_item & operator = (timer_item const &) = delete;
    };

    // Comparison functor to sort the timer "queue" by Timer::next
    struct next_active_comparator
    {
        bool operator () (timer_item const & a, timer_item const & b) const noexcept
        {
            return a.next < b.next;
        }
    };

    using timer_map = std::unordered_map<timer_id, timer_item>;

    // Queue is a set of references to timer_item objects, sorted by next
    using value_type = std::reference_wrapper<timer_item>;
    using timer_queue = std::multiset<value_type, next_active_comparator>;

private:
    // One worker thread for an unlimited number of timers is acceptable
    // Lazily started when first timer is started
    // TODO: Implement auto-stopping the timer thread when it is idle for
    // a configurable period.
    mutable mutex_type _mtx;
    std::thread _worker;

    // Inexhaustible source of unique IDs
    timer_id _next_id;

    // The Timer objects are physically stored in this map
    timer_map _active;

    // The ordering queue holds references to items in '_active'
    timer_queue _queue;

    condition_variable_type _wakeup_cv;

    std::atomic_bool _done{false};

    // Valid IDs are guaranteed not to be this value
    static timer_id constexpr no_timer = timer_id{0};

public:
    // Constructor does not start worker until there is a Timer.
    timer_pool () : _next_id(no_timer + 1)
    {}

    // Destructor is thread safe, even if a timer callback is running.
    // All callbacks are guaranteed to have returned before this
    // destructor returns.
    ~timer_pool ()
    {
        locker_type locker(_mtx);

        // The worker might not be running
        if (_worker.joinable()) {
            _done = true;
            locker.unlock();
            _wakeup_cv.notify_all();

            // If a timer callback is running, this
            // will make sure it has returned before
            // allowing any deallocations to happen
            _worker.join();

            // Note that any timers still in the queue
            // will be destructed properly but they
            // will not be invoked
        }
    }

    /**
      Create a new timer using milliseconds, and add it to the internal queue.

      @param delay  Callback starts firing this many seconds from now
      @param period If non-zero, callback is fired again after this period in seconds
      @param func   The callback to run at the specified interval

      @return  Id used to identify the timer for later use
    */
    timer_id create (std::chrono::milliseconds delay
        , std::chrono::milliseconds period
        , callback_type && callback)
    {
        locker_type locker(_mtx);

        // Lazily start thread when first timer is requested
        if (!_worker.joinable())
            _worker = std::thread(& timer_pool::worker, this);

        // Assign an ID and insert it into function storage
        auto id = _next_id++;

        // Insert a reference to the Timer into ordering queue
        auto iter = _active.emplace(id
            , timer_item(id
                , clock_type::now() + delay
                , period
                , std::move(callback)));

        // Insert a reference to the Timer into ordering queue
        typename timer_queue::iterator place = _queue.emplace(iter.first->second);

        // We need to notify the timer thread only if we inserted
        // this timer into the front of the timer queue
        bool need_notify = (place == _queue.begin());

        locker.unlock();

        if (need_notify)
            _wakeup_cv.notify_all();

        return id;
    }

    timer_id create (double delay, double period, callback_type && callback)
    {
        assert(delay * 1000 <= static_cast<decltype(delay)>(std::numeric_limits<intmax_t>::max()));
        assert(period * 1000 <= static_cast<decltype(period)>(std::numeric_limits<intmax_t>::max()));
        auto delay_millis = duration_millis_type(static_cast<intmax_t>(delay * 1000));
        auto period_millis = duration_millis_type(static_cast<intmax_t>(period * 1000));

        return create(delay_millis, period_millis, std::move(callback));
    }

    /**
     * Overloaded method. Creates singleshot timer
     */
    inline timer_id create (std::chrono::milliseconds timeout
        , callback_type && callback)
    {
        return create(timeout, std::chrono::milliseconds{0}, std::move(callback));
    }

    /**
     * Overloaded method. Creates singleshot timer
     */
    inline timer_id create (double timeout, callback_type && callback)
    {
        return create(timeout, double{0}, std::move(callback));
    }

    /**
     * Destroy the specified timer.
     *
     * Synchronizes with the worker thread if the callback for this timer
     * is running, which guarantees that the callback for that callback is
     * not running before clear() returns.
     *
     * You are not required to clear any timers. You can forget their
     * TimerId if you do not need to cancel them.
     *
     * The only time you need this is when you want to stop a timer that
     * has a repetition period, or you want to cancel a timeout that has
     * not fired yet.
     */
    bool destroy (timer_id id)
    {
        locker_type locker(_mtx);
        auto it = _active.find(id);
        return destroy_helper(locker, it, true);
    }

    /**
     * Destroy all timers, but preserve id uniqueness.
     * This carefully makes sure every timer is not executing its callback
     * before destructing it.
    */
    void destroy_all ()
    {
        locker_type locker(_mtx);

        while (!_active.empty())
            destroy_helper(locker, _active.begin(), _queue.size() == 1);
    }

    std::size_t size () const noexcept
    {
        locker_type locker(_mtx);
        return _active.size();
    }

    bool empty () const noexcept
    {
        locker_type locker(_mtx);
        return _active.empty();
    }

private:
    void worker ()
    {
        locker_type locker(_mtx);

        while (!_done) {
            if (_queue.empty()) {
                // Wait for done or work
                _wakeup_cv.wait(locker, [this] { return _done || !_queue.empty(); });
                continue;
            }

            auto head = _queue.begin();
            timer_item & timer = *head;
            auto now = clock_type::now();

            if (now >= timer.next) {
                _queue.erase(head);

                // Mark it as running to handle racing destroy
                timer.running = true;

                // Call the callback outside the lock
                locker.unlock();
                timer.callback();
                locker.lock();

                if (timer.running) {
                    timer.running = false;

                    // If it is periodic, schedule a new one
                    if (timer.period.count() > 0) {
                        timer.next = timer.next + timer.period;
                        _queue.emplace(timer);
                    } else {
                        // Not rescheduling, destruct it
                        _active.erase(timer.id);
                    }
                } else {
                    // timer.running changed!
                    //
                    // Running was set to false, destroy was called
                    // for this Timer while the callback was in progress
                    // (this thread was not holding the lock during the callback)
                    // The thread trying to destroy this timer is waiting on
                    // a condition variable, so notify it
                    timer.wait_cv->notify_all();

                    // The clearTimer call expects us to remove the instance
                    // when it detects that it is racing with its callback
                    _active.erase(timer.id);
                }
            } else {
                // Wait until the timer is ready or a timer creation notifies
                time_point_type next = timer.next;
                _wakeup_cv.wait_until(locker, next);
            }
        }
    }

    bool destroy_helper (locker_type & locker, typename timer_map::iterator it, bool notify)
    {
        assert(locker.owns_lock());

        if (it == _active.end())
            return false;

        timer_item & timer = it->second;

        if (timer.running) {
            // A callback is in progress for this Timer,
            // so flag it for deletion in the worker
            timer.running = false;

            // Assign a condition variable to this timer
            timer.wait_cv.reset(new condition_variable_type);

            // Block until the callback is finished
            if (std::this_thread::get_id() != _worker.get_id())
                timer.wait_cv->wait(locker);
        } else {
            _queue.erase(timer);
            _active.erase(it);

            if (notify) {
                locker.unlock();
                _wakeup_cv.notify_all();
            }
        }

        return true;
    }
};

} // namespace pfs
