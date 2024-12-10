////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2024 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2024.12.10 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "namespace.hpp"
#include <mutex>

PFS__NAMESPACE_BEGIN

template <typename T, typename Mutex = std::mutex
    , template <typename...> class WriterLocker = std::unique_lock
    , template <typename...> class ReaderLocker = WriterLocker>
class synchronized
{
public:
    template <typename U, typename L>
    class basic_guard
    {
    protected:
        L _locker;
        U & _ref;

    public:
        basic_guard (U & ref, Mutex & mtx)
            : _locker(mtx)
            , _ref(ref)
        {}

        basic_guard (basic_guard const &) = delete;
        basic_guard (basic_guard &&) = default;
        basic_guard & operator = (basic_guard const &) = delete;
        basic_guard & operator = (basic_guard &&) = delete;

        ~basic_guard ()
        {
            _locker.unlock();
        }
    };

    template <typename U>
    class reader_guard: public basic_guard<U, ReaderLocker<Mutex>>
    {
    public:
        using basic_guard<U, ReaderLocker<Mutex>>::basic_guard;
    public:
        U * operator -> () const noexcept
        {
            return & this->_ref;
        }

        U & operator * () const noexcept
        {
            return this->_ref;
        }
    };

    template <typename U>
    class writer_guard: public basic_guard<U, WriterLocker<Mutex>>
    {
    public:
        using basic_guard<U, WriterLocker<Mutex>>::basic_guard;

    public:
        U * operator -> () const noexcept
        {
            return & this->_ref;
        }

        U & operator * () noexcept
        {
            return this->_ref;
        }
    };

    using reader_guard_type = reader_guard<T>;
    using writer_guard_type = writer_guard<T>;

private:
    T _value;
    mutable Mutex _mtx;
    mutable Mutex _shared_mtx;

public:
    template <typename ...Args>
    explicit synchronized (Args &&... args)
        : _value(std::forward<Args>(args)...)
    {}

public:
    writer_guard<T> wlock ()
    {
        std::lock_guard<Mutex> locker{_mtx};
        return writer_guard<T>{_value, _shared_mtx};
    }

    reader_guard<T const> rlock () const
    {
        std::lock_guard<Mutex> locker{_mtx};
        return reader_guard<T const>{_value, _shared_mtx};
    }

    T & unsafe () noexcept
    {
        return _value;
    }

    T const & unsafe () const noexcept
    {
        return _value;
    }
};

PFS__NAMESPACE_END
