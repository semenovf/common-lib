////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2020.10.21 Initial version
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "pfs/iterator.hpp"
#include <condition_variable>
#include <limits>
#include <list>
#include <mutex>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace pfs {

namespace ring_buffer_details {

template <typename Container>
bool contains_iterator (typename Container::iterator it);

template <typename T, std::size_t N>
class static_vector
{
public:
    using iterator = typename std::aligned_storage<sizeof(T), alignof(T)>::type *;
    using const_iterator = typename std::aligned_storage<sizeof(T), alignof(T)>::type const *;

private:
    typename std::aligned_storage<sizeof(T), alignof(T)>::type _data[N];
    std::size_t _size = 0;

public:
    ~static_vector ()
    {
        // Deleting elements is a responsibility of the holder
    }

    // For test purposes only
    T const & operator [] (std::size_t pos) const
    {
        return *reinterpret_cast<T const *>(& _data[pos]);
    }

    // For test purposes only
    T & operator [] (std::size_t pos)
    {
        return *reinterpret_cast<T *>(& _data[pos]);
    }

    iterator begin ()
    {
        return & _data[0];
    }

    const_iterator begin () const
    {
        return & _data[0];
    }

    iterator end ()
    {
        return & _data[N];
    }

    const_iterator end () const
    {
        return & _data[N];
    }

    bool contains_iterator (iterator it) const
    {
        return it >= & _data[0] && it < & _data[N];
    }

    bool contains_iterator (const_iterator it) const
    {
        return it >= & _data[0] && it < & _data[N];
    }

    // Move values inclusive from first to last
    static iterator move_values (iterator first, iterator last, iterator pos)
    {
        auto last_pos = pos;
        ++last;

        for (auto it = first; it != last; ++it) {
            new (& *pos) T(std::move(*reinterpret_cast<T *>(it)));
            last_pos = pos;
            ++pos;
        }

        return last_pos;
    }
};

template <typename BulksBuffer>
class forward_iterator : public pfs::iterator_facade<
          pfs::forward_iterator_tag
        , forward_iterator<BulksBuffer>
        , typename BulksBuffer::value_type
        , typename BulksBuffer::value_type *
        , typename BulksBuffer::value_type &>
{
    using item_iterator = typename BulksBuffer::item_iterator;
    using bulk_iterator = typename BulksBuffer::bulk_iterator;

    BulksBuffer * _pholder{nullptr};

    bulk_iterator _bulk_pos;
    item_iterator _pos;

public:
    forward_iterator ()
    {}

    forward_iterator (BulksBuffer & holder
        , bulk_iterator bulk_pos
        , item_iterator pos)
        : _pholder(& holder)
        , _bulk_pos(bulk_pos)
        , _pos(pos)
    {}

    forward_iterator (forward_iterator const & other)
        : _pholder(other._pholder)
        , _bulk_pos(other._bulk_pos)
        , _pos(other._pos)
    {}

    forward_iterator & operator = (forward_iterator const & other)
    {
        _pholder = other._pholder;
        _bulk_pos = other._bulk_pos;
        _pos = other._pos;
        return *this;
    }

    typename forward_iterator::reference ref ()
    {
        return *reinterpret_cast<typename forward_iterator::pointer>(& *_pos);
    }

    typename forward_iterator::pointer ptr ()
    {
        return reinterpret_cast<typename forward_iterator::pointer>(& *_pos);
    }

    void increment (typename forward_iterator::difference_type)
    {
        ++_pos;

        // End of bulk exceeded
        if (_pos == _bulk_pos->end()) {
            // Has next bulk
            if (_bulk_pos != _pholder->last_bulk()) {
                ++_bulk_pos;
                _pos = _bulk_pos->begin();
            }
        }
    }

    bool equals (forward_iterator const & other) const
    {
        return _pos == other._pos;
    }

    typename BulksBuffer::item_iterator base ()
    {
        return _pos;
    }

    bulk_iterator current_bulk () const
    {
        return _bulk_pos;
    }
};

template <typename T, size_t N>
using default_bulk_container = static_vector<T, N>;

template <typename T>
using default_list_container = std::list<T>;

} // namespace ring_buffer_details

///////////////////////////////////////////////////////////
// ring_buffer
///////////////////////////////////////////////////////////
template <typename T
    , size_t BULK_SIZE
    , size_t MAX_BUFFER_SIZE = (std::numeric_limits<std::size_t>::max)()
    , template <typename> class ListContainer = ring_buffer_details::default_list_container
    , template <typename, size_t> class BulkContainer = ring_buffer_details::default_bulk_container>
class ring_buffer
{
    friend class ring_buffer_details::forward_iterator<ring_buffer>;

public:
    using value_type = T;
    using reference = T &;
    using const_reference = T const &;
    using pointer = T *;
    using const_pointer = T const *;
    using bulk_type = BulkContainer<T, BULK_SIZE>;
    using bulk_list_type = ListContainer<bulk_type>;
    using size_type = std::size_t;

    // Actually infinite limit
    static constexpr const size_type nbulks = (std::numeric_limits<size_type>::max)();
    static constexpr const size_type bulk_size = BULK_SIZE;

private:
    using item_iterator = typename bulk_type::iterator;
    using bulk_iterator = typename bulk_list_type::iterator;

    using iterator = ring_buffer_details::forward_iterator<ring_buffer>;

private:
    bulk_list_type _bulks;

    // Use this memeber for constant complexity call of size()
    size_type _size {0};

    iterator _head;
    iterator _tail;
    bool _is_head_preceed_tail {true};

public:
    /**
     * Constructs container with specified bulk size and with limit of bulks.
     */
    ring_buffer (size_type bulk_count)
    {
        if (!bulk_count)
            bulk_count = 1;

        // Initialize list of bulks
        for (size_t i = 0; i < bulk_count; i++)
            _bulks.emplace_back();

        _head = begin();
        _tail = end();
    }

    /**
     * Constructs container with default bulk size (@see ring_buffer::default_bulk_size)
     * and with only one bulk.
     */
    ring_buffer ()
        : ring_buffer(1)
    {}

    ring_buffer (ring_buffer const & other) = delete;
    ring_buffer & operator = (ring_buffer const & other) = delete;

    ring_buffer (ring_buffer && other)
        : _bulks(std::move(other._bulks))
        , _size(other._size)
        , _head(other._head)
        , _tail(other._tail)
    {
        other._size = 0;
    }

    ring_buffer & operator = (ring_buffer && other)
    {
        _bulks = std::move(other._bulks);
        _size = other._size;
        _head = other._head;
        _tail = other._tail;
        _is_head_preceed_tail = true;

        other._size = 0;

        return *this;
    }

    ~ring_buffer ()
    {
        clear();
    }

    //
    // Capacity
    //

    /**
     * Checks if buffer has no elements.
     */
    bool empty () const
    {
        return _size == 0;
    }

    /**
     * Capacity of ther buffer.
     */
    size_type capacity () const
    {
        return bulk_size * _bulks.size();
    }

    /**
     * The number of elements in the buffer.
     */
    size_type size () const
    {
        return _size;
    }

    // For test purposes only
    size_type bulk_count () const
    {
        return _bulks.size();
    }

    /**
     * Increase the capacity of the buffer to a value that's greater or equal
     * to @a new_capacity. If @a new_capacity is greater than the current
     * ring_buffer::capacity(), new storage is allocated, otherwise the method
     * does nothing.
     * ring_buffer::reserve() does not change the size of the vector.
     */
    void reserve (size_type new_capacity)
    {
        if (new_capacity <= capacity())
            return;

        // Calculate number of bulks to add
        size_type nbulks = (new_capacity - 1 - capacity()) / bulk_size + 1;

        if (_size == 0) {
            for (size_type i = 0; i < nbulks; i++)
                _bulks.emplace_back();
            _head = begin();
            _tail = end();
        } else {
            // If head is on the left side from tail just add new bulks to the end
            if (_is_head_preceed_tail) {
                for (size_type i = 0; i < nbulks; i++)
                    _bulks.emplace_back();
            } else {
                // If head is on the right side from tail

                // If head and tail is on different bulks just insert new bulks
                // before head's bulk
                if (!_head.current_bulk()->contains_iterator(_tail.base())) {
                    for (size_type i = 0; i < nbulks; i++)
                        _bulks.emplace(_head.current_bulk());
                } else {
                    // If head and tail is on the same bulks insert new bulks
                    // before head's bulk and move values to the new bulk

                    // Save first inserted bulk to store tail's values from head's bulk
                    auto first_inserted_bulk = _bulks.emplace(_head.current_bulk());

                    // Insert the rest bulks
                    for (size_type i = 0; i < nbulks - 1; i++)
                        _bulks.emplace(_head.current_bulk());

                    // Get first and last value positions to move
                    auto first = _tail.current_bulk()->begin();
                    auto last = _tail.base();
                    auto pos = bulk_type::move_values(first, last, first_inserted_bulk->begin());
                    _tail = iterator(*this, first_inserted_bulk, pos);
                }
            }
        }
    }

    //
    // Element access
    //

    /**
     * Returns reference to the first element in the queue.
     * This element will be the first element to be removed on a call to pop().
     *
     * @exception std::out_of_range.
     */
    reference front ()
    {
        if (_size == 0)
            throw std::out_of_range("ring_buffer::front()");

        return *reinterpret_cast<pointer>(& *_head);
    }

    /**
     * Returns const reference to the first element in the queue.
     * This element will be the first element to be removed on a call to pop().
     *
     * @exception std::out_of_range.
     */
    const_reference front () const
    {
        if (_size == 0)
            throw std::out_of_range("ring_buffer::front()");

        return *reinterpret_cast<const_pointer>(& *_head);
    }

    /**
     * @exception std::out_of_range.
     */
    reference back ()
    {
        if (_size == 0)
            throw std::out_of_range("ring_buffer::back()");

        return *reinterpret_cast<pointer>(& *_tail);
    }

    /**
     * @exception std::out_of_range.
     */
    const_reference back () const
    {
        if (_size == 0)
            throw std::out_of_range("ring_buffer::back()");

        return *reinterpret_cast<const_pointer>(& *_tail);
    }

    //
    // Modifiers
    //

    void clear ()
    {
        if (_size == 0)
            return;

        while (_head != _tail) {
            reinterpret_cast<pointer>(& *_head)->~value_type();
            ++_head;

            if (_head == end())
                _head = begin();
        }
        
		// NOTE: Exception on Windows
		// NOTE: Success on Linux
		// Destroy last element
        //reinterpret_cast<pointer>(& *_head)->~value_type();

        _head = begin();
        _tail = end();
        _is_head_preceed_tail = true;

        _size = 0;
    }

    /**
     * @exception std::bad_alloc no more space available.
     */
    void push (value_type const & value)
    {
        ensure_capacity();
        move_write_position();
        new (& *_tail) value_type(value);
    }

    /**
     * @exception std::bad_alloc no more space available.
     */
    void push (value_type && value)
    {
        ensure_capacity();
        move_write_position();
        new (& *_tail) value_type(std::forward<value_type>(value));
    }

    template <typename ...Args>
    void emplace (Args &&... args)
    {
        ensure_capacity();
        move_write_position();
        new (& *_tail) value_type(std::forward<Args>(args)...);
    }

    void pop ()
    {
        if (_size == 0)
            return;

        //_head->~value_type();
        reinterpret_cast<pointer>(& *_head)->~value_type();

        --_size;

        if (_size == 0) {
            _head = begin();
            _tail = end();
            _is_head_preceed_tail = true;
        } else {
            ++_head;

            if (_head == end()) {
                _head = begin();
                _is_head_preceed_tail = true;
            }
        }
    }

private:
    void move_write_position ()
    {
        // Initially _tail points to the end of ring buffer
        if (_tail == end()) {
            _tail = begin();

            if (_size > 0) // is not first movement
                _is_head_preceed_tail = false;
        } else {
            ++_tail;

            if (_tail == end()) {
                _tail = begin();

                if (_size > 0) // is not first movement
                    _is_head_preceed_tail = false;
            }
        }

        _size++;
    }

    inline iterator begin ()
    {
        return iterator(*this
            , _bulks.begin()
            , _bulks.front().begin());
    }

    inline iterator end ()
    {
        return iterator(*this
            , last_bulk()
            , _bulks.back().end());
    }

    inline iterator last ()
    {
        return iterator(*this
            , last_bulk()
            , --_bulks.back().end());
    }

    // Iterator specific helper method
    inline bulk_iterator last_bulk ()
    {
        return --_bulks.end();
    }

    inline void ensure_capacity ()
    {
        if (size() == capacity()) {
            if (size() >= MAX_BUFFER_SIZE)
                throw std::bad_alloc();

            auto new_size = size() + BULK_SIZE;

            if (new_size > MAX_BUFFER_SIZE)
                new_size = MAX_BUFFER_SIZE;

            reserve(new_size);
        }
    }
};

///////////////////////////////////////////////////////////
// ring_buffer_mt
///////////////////////////////////////////////////////////
template <typename T
    , size_t BULK_SIZE
    , typename BasicLockable = std::mutex
    , typename ConditionVariable = std::condition_variable
    , template <typename> class ListContainer = ring_buffer_details::default_list_container
    , template <typename, size_t> class BulkContainer = ring_buffer_details::default_bulk_container
    , size_t MAX_BUFFER_SIZE = (std::numeric_limits<std::size_t>::max)()>
class ring_buffer_mt : protected ring_buffer<T, BULK_SIZE, MAX_BUFFER_SIZE, ListContainer
    , BulkContainer>
{
    using base_class = ring_buffer<T, BULK_SIZE, MAX_BUFFER_SIZE, ListContainer, BulkContainer>;
    using mutex_type = BasicLockable;
    using condition_variable_type = ConditionVariable;

public:
    using value_type = typename base_class::value_type;
    using size_type = typename base_class::size_type;

private:
    mutable mutex_type _mtx;
    mutable condition_variable_type _condvar;

public:
    using base_class::base_class;

    //
    // Capacity
    //

    /**
     * Checks if buffer has no elements.
     */
    bool empty () const
    {
        std::unique_lock<mutex_type> locker{_mtx};
        return base_class::empty();
    }

    /**
     * Capacity of ther buffer.
     */
    size_type capacity () const
    {
        std::unique_lock<mutex_type> locker{_mtx};
        return base_class::capacity();
    }

    /**
     * The number of elements in the buffer.
     */
    size_type size () const
    {
        std::unique_lock<mutex_type> locker{_mtx};
        return base_class::size();
    }

    // For test purposes only
    size_type bulk_count () const
    {
        std::unique_lock<mutex_type> locker{_mtx};
        return base_class::bulk_count();
    }

    void reserve (size_type new_capacity)
    {
        std::unique_lock<mutex_type> locker{_mtx};
        base_class::reserve(new_capacity);
    }

    //
    // Modifiers
    //

    void clear ()
    {
        std::unique_lock<mutex_type> locker{_mtx};
        return base_class::clear();
    }

    /**
     * @exception std::bad_alloc no more space available.
     */
    void push (value_type const & value)
    {
        std::unique_lock<mutex_type> locker{_mtx};
        base_class::push(value);
        _condvar.notify_one();
    }

    /**
     * @exception std::bad_alloc no more space available.
     */
    void push (value_type && value)
    {
        std::unique_lock<mutex_type> locker{_mtx};
        base_class::push(std::move(value));
        _condvar.notify_one();
    }

    template <typename ...Args>
    void emplace (Args &&... args)
    {
        std::unique_lock<mutex_type> locker{_mtx};
        base_class::emplace(std::forward<Args>(args)...);
        _condvar.notify_one();
    }

    void pop ()
    {
        std::unique_lock<mutex_type> locker{_mtx};
        base_class::pop();
    }

    //
    // Multithread-specific convenient methods
    //

    bool try_push (value_type const & value, size_type capacity_inc = 0)
    {
        std::unique_lock<mutex_type> locker{_mtx};

        if (base_class::size() == base_class::capacity()) {
            if (!capacity_inc)
                return false;

            base_class::reserve(base_class::capacity() + capacity_inc);
        }

        base_class::push(value);
        _condvar.notify_one();
        return true;
    }

    /**
     * @exception std::bad_alloc no more space available.
     */
    bool try_push (value_type && value, size_type capacity_inc = 0)
    {
        std::unique_lock<mutex_type> locker{_mtx};

        if (base_class::size() == base_class::capacity()) {
            if (!capacity_inc)
                return false;

            base_class::reserve(base_class::capacity() + capacity_inc);
        }

        base_class::push(std::forward<value_type>(value));
        _condvar.notify_one();
        return true;
    }

    template <typename ...Args>
    bool try_emplace (Args &&... args)
    {
        std::unique_lock<mutex_type> locker{_mtx};

        if (base_class::size() == base_class::capacity())
            return false;

        base_class::emplace(std::forward<Args>(args)...);
        _condvar.notify_one();
        return true;
    }

    template <typename ...Args>
    bool try_reserve_and_emplace (size_type capacity_inc, Args &&... args)
    {
        std::unique_lock<mutex_type> locker{_mtx};

        if (base_class::size() == base_class::capacity()) {
            if (!capacity_inc)
                return false;

            base_class::reserve(base_class::capacity() + capacity_inc);
        }

        base_class::emplace(std::forward<Args>(args)...);
        _condvar.notify_one();
        return true;
    }

    bool try_pop (value_type & value)
    {
        std::unique_lock<mutex_type> locker{_mtx};

        if (!base_class::size())
            return false;

        value = std::move(base_class::front());
        base_class::pop();
        return true;
    }

    void wait ()
    {
        std::unique_lock<mutex_type> locker{_mtx};

        while (base_class::empty()) {
            _condvar.wait(locker, [this] {
                return !base_class::empty();
            });
        }
    }

    template <typename Rep, typename Period>
    void wait_for (std::chrono::duration<Rep, Period> const & rel_time)
    {
        std::unique_lock<mutex_type> locker{_mtx};

        if (base_class::empty()) {
            _condvar.wait_for(locker, rel_time, [this] {
                return !base_class::empty();
            });
        }
    }
};

} // namespace pfs
