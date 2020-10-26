////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of [pfs-common](https://github.com/semenovf/pfs-common) library.
//
// Changelog:
//      2020.10.21 Initial version
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "pfs/iterator.hpp"
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

    static void move_values (iterator first, iterator last, iterator pos)
    {
        for (auto it = first; it != last; ++it) {
            *reinterpret_cast<T *>(pos) = std::move(*reinterpret_cast<T *>(it));
            ++pos;
        }
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

template <typename T
    , size_t BulkSize
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
    using bulk_type = BulkContainer<T, BulkSize>;
    using bulk_list_type = ListContainer<bulk_type>;
    using size_type = std::size_t;

    // Actually infinite limit
    static constexpr const size_type nbulks = std::numeric_limits<size_type>::max();
    static constexpr const size_type bulk_size = BulkSize;

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

    bool empty () const
    {
        return _size == 0;
    }

    size_type capacity () const
    {
        return bulk_size * _bulks.size();
    }

    size_type size () const
    {
        return _size;
    }

    size_type bulk_count () const
    {
        return _bulks.size();
    }

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
                    ++last;

                    bulk_type::move_values(first, last
                        , first_inserted_bulk->begin());
                }
            }
        }
    }


//     void splice (ring_buffer && other)
//     {
//         // Both are empty
//         if (_size == 0 && other._size == 0) {
//             _bulks.splice(_bulks.end(), std::forward<bulk_list_type>(other._bulks));
//             _capacity += other._capacity;
//             _head = begin();
//             _tail = end();
//         } else {
//             if (other._size == 0) {
//                 // Check if head is on the left side from tail
//                 if (_is_head_preceed_tail) {
//                     _bulks.splice(_bulks.end(), std::forward<bulk_list_type>(other._bulks));
//                     _capacity += other._capacity;
//                 } else {
//                     // TODO
//                 }
//             } else {
//                 throw std::logic_error("ring_buffer::splice(): ring_buffers can be spliced if:\n"
//                     "\t* both buffers are empty\n"
//                     "\t* head of first buffer is at the left side from tail and second buffer is empty");
//             }
//         }
//
//         other._size = 0;
//         other._capacity = 0;
//     }

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

        // Destroy last element
        reinterpret_cast<pointer>(& *_head)->~value_type();

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
        if (_size == capacity())
            throw std::bad_alloc();

        move_write_position();

        new (& *_tail) value_type(value);
    }

    /**
     * @exception std::bad_alloc no more space available.
     */
    void push (value_type && value)
    {
        if (_size == capacity())
            throw std::bad_alloc();

        move_write_position();

        new (& *_tail) value_type(std::forward<value_type>(value));
    }

    template <typename ...Args>
    void emplace (Args &&... args)
    {
        if (_size == capacity())
            throw std::bad_alloc();

        move_write_position();
        new (& *_tail) value_type(std::forward<Args>(args)...);
    }

    void pop ()
    {
        if (_size == 0)
            return;

        //_head->~value_type();
        reinterpret_cast<pointer>(& *_head)->~value_type();

        ++_head;

        if (_head == end()) {
            _head = begin();
            _is_head_preceed_tail = true;
        }

        --_size;
    }

private:
    void move_write_position ()
    {
        // Intially _tail points to the end of ring buffer
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

    iterator begin ()
    {
        return iterator(*this
            , _bulks.begin()
            , _bulks.front().begin());
    }

    iterator end ()
    {
        return iterator(*this
            , last_bulk()
            , _bulks.back().end());
    }

    iterator last ()
    {
        return iterator(*this
            , last_bulk()
            , --_bulks.back().end());
    }

    // Iterator specific helper method
    bulk_iterator last_bulk ()
    {
        return --_bulks.end();
    }
};


template <typename T
    , size_t BulkSize
    , typename BasicLockable = std::mutex
    , template <typename> class ListContainer = ring_buffer_details::default_list_container
    , template <typename, size_t> class BulkContainer = ring_buffer_details::default_bulk_container>
class ring_buffer_mt : protected ring_buffer<T, BulkSize, ListContainer, BulkContainer>
{
    using base_class = ring_buffer<T, BulkSize, ListContainer, BulkContainer>;
    using mutex_type = BasicLockable;

public:
    using value_type = typename base_class::value_type;
    using size_type = typename base_class::size_type;

private:
    mutable mutex_type _mtx;

public:
    using base_class::base_class;

    bool empty () const
    {
        std::unique_lock<mutex_type> locker{_mtx};
        return base_class::empty();
    }

    size_type capacity () const
    {
        std::unique_lock<mutex_type> locker{_mtx};
        return base_class::capacity();
    }

    size_type size () const
    {
        std::unique_lock<mutex_type> locker{_mtx};
        return base_class::size();
    }

    size_type bulk_count () const
    {
        std::unique_lock<mutex_type> locker{_mtx};
        return base_class::bulk_count();
    }

    void clear ()
    {
        std::unique_lock<mutex_type> locker{_mtx};
        return base_class::clear();
    }

    void reserve (size_type new_capacity)
    {
        std::unique_lock<mutex_type> locker{_mtx};
        base_class::reserve(new_capacity);
    }

    bool try_push (value_type const & value, size_type new_capacity = 0)
    {
        std::unique_lock<mutex_type> locker{_mtx};

        if (base_class::size() == base_class::capacity()) {
            if (new_capacity <= base_class::capacity())
                return false;

            base_class::reserve(new_capacity);
        }

        base_class::push(value);
        return true;
    }

    /**
     * @exception std::bad_alloc no more space available.
     */
    bool try_push (value_type && value, size_type new_capacity = 0)
    {
        std::unique_lock<mutex_type> locker{_mtx};

        if (base_class::size() == base_class::capacity()) {
            if (new_capacity <= base_class::capacity())
                return false;

            base_class::reserve(new_capacity);
        }

        base_class::push(std::forward<value_type>(value));
        return true;
    }

    template <typename ...Args>
    bool try_emplace (Args &&... args)
    {
        std::unique_lock<mutex_type> locker{_mtx};

        if (base_class::size() == base_class::capacity())
            return false;

        base_class::emplace(std::forward<Args>(args)...);
        return true;
    }

    template <typename ...Args>
    bool try_reserve_and_emplace (size_type new_capacity, Args &&... args)
    {
        std::unique_lock<mutex_type> locker{_mtx};

        if (base_class::size() == base_class::capacity()) {
            if (new_capacity <= base_class::capacity())
                return false;

            base_class::reserve(new_capacity);
        }

        base_class::emplace(std::forward<Args>(args)...);
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
};

} // namespace pfs
