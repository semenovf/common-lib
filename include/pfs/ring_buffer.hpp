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
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace pfs {

namespace ring_buffer_details {

template <typename T>
using default_bulk_container = std::vector<T>;

template <typename T>
using default_list_container = std::list<T>;

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
        //return *_pos;
        return *reinterpret_cast<typename forward_iterator::pointer>(& *_pos);
    }

    typename forward_iterator::pointer ptr ()
    {
        //return & *_pos;
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
};

} // namespace ring_buffer_details

template <typename T
    , template <typename> class ListContainer = ring_buffer_details::default_list_container
    , template <typename> class BulkContainer = ring_buffer_details::default_bulk_container>
class ring_buffer
{
    friend class ring_buffer_details::forward_iterator<ring_buffer>;

public:
    using item_storage = std::aligned_storage<sizeof(T), alignof(T)>;
    using value_type = T;
    using reference = T &;
    using const_reference = T const &;
    using pointer = T *;
    using const_pointer = T const *;
    using bulk_type = BulkContainer<item_storage>;
    using bulk_list_type = ListContainer<bulk_type>;
    using size_type = std::size_t;

    // Actually infinite limit
    static constexpr const size_type nbulks = std::numeric_limits<size_type>::max();
    static constexpr const size_type default_bulk_size = 256;

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

public:
    /**
     * Constructs container with specified bulk size and with limit of bulks.
     */
    ring_buffer (size_type bulk_size, size_type bulk_count = 1)
    {
        if (!bulk_size)
            bulk_size = default_bulk_size;

        if (!bulk_count)
            bulk_count = 1;

        // Initialize list of bulks
        for (size_t i = 0; i < bulk_count; i++) {
            _bulks.emplace_back();
            _bulks.back().resize(bulk_size);
        }

        _head = begin();
        _tail = end();
    }

    /**
     * Constructs container with default bulk size (@see ring_buffer::default_bulk_size)
     * and with only one bulk.
     */
    ring_buffer ()
        : ring_buffer(default_bulk_size, 1)
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
        return bulk_size() * bulk_count();
    }

    size_type size () const
    {
        return _size;
    }

    size_type bulk_size () const
    {
        return _bulks.front().size();
    }

    size_type bulk_count () const
    {
        return _bulks.size();
    }

    void splice (ring_buffer && other)
    {
        // Both are empty
        if (_size == 0 && other._size == 0) {
            _bulks.splice(_bulks.end(), std::forward<bulk_list_type>(other._bulks));
            _head = begin();
            _tail = end();
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
        }

        // Destroy last element
        reinterpret_cast<pointer>(& *_head)->~value_type();

        _head = begin();
        _tail = end();

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

        if (_head == end())
            _head = begin();

        --_size;
    }

private:
    void move_write_position ()
    {
        if (_tail == end())
            _tail = begin();
        else
            ++_tail;

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

    // Iterator specific helper method
    bulk_iterator last_bulk ()
    {
        return --_bulks.end();
    }
};

template <typename T
    , template <typename> class ListContainer
    , template <typename> class BulkContainer>
typename ring_buffer<T, ListContainer, BulkContainer>::size_type const
ring_buffer<T, ListContainer, BulkContainer>::default_bulk_size;

} // namespace pfs
