////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2019.12.16 Initial version (inspired from https://github.com/semenovf/pfs)
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <iterator>

namespace pfs {

using input_iterator_tag = std::input_iterator_tag;
using output_iterator_tag = std::output_iterator_tag;
using forward_iterator_tag = std::forward_iterator_tag;
using bidirectional_iterator_tag = std::bidirectional_iterator_tag;
using random_access_iterator_tag = std::random_access_iterator_tag;

template <typename Category, typename T, typename Distance = std::ptrdiff_t,
    typename Pointer = T *, typename Reference = T &>
struct iterator
{
    using iterator_category = Category;
    using value_type = T;
    using difference_type = Distance;
    using pointer = Pointer;
    using reference = Reference;
};

template <typename Iterator>
struct iterator_traits
{
    using iterator_category = typename Iterator::iterator_category;
    using value_type = typename Iterator::value_type;
    using difference_type = typename Iterator::difference_type;
    using pointer = typename Iterator::pointer;
    using reference = typename Iterator::reference;
};

template <typename T>
struct iterator_traits <T *>
{
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T *;
    using reference = T &;
};

template <typename T>
struct iterator_traits<T const *>
{
    typedef std::random_access_iterator_tag iterator_category;
    typedef T         value_type;
    typedef std::ptrdiff_t difference_type;
    typedef const T * pointer;
    typedef const T & reference;
};

template <typename Category
        , typename Derived
        , typename T
        , typename Pointer
        , typename Reference
        , typename Distance = std::ptrdiff_t>
struct iterator_facade;

template <typename Derived
        , typename T
        , typename Pointer
        , typename Reference
        , typename Distance>
class iterator_facade<input_iterator_tag, Derived, T, Pointer, Reference, Distance>
{
public:
    using iterator_category = input_iterator_tag;
    using value_type = T;
    using difference_type = Distance;
    using pointer = Pointer;
    using reference = Reference;

    value_type operator * () const
    {
        return const_cast<Derived *>(static_cast<Derived const *>(this))->ref();
    }

    pointer operator -> () const
    {
        return const_cast<Derived *>(static_cast<Derived const *>(this))->ptr();
    }

    Derived & operator ++ () // prefix increment
    {
        static_cast<Derived *>(this)->increment(1);
        return *static_cast<Derived *>(this);
    }

    Derived operator ++ (int) // postfix increment
    {
        Derived r(*static_cast<Derived *>(this));
        static_cast<Derived *>(this)->increment(1);
        return r;
    }

    friend inline bool operator == (Derived const & lhs, Derived const & rhs)
    {
        return lhs.equals(rhs);
    }

    friend inline bool operator != (Derived const & lhs, Derived const & rhs)
    {
        return ! lhs.equals(rhs);
    }
};

template <typename Derived
        , typename T
        , typename Pointer
        , typename Reference
        , typename Distance>
class iterator_facade<output_iterator_tag, Derived, T, Pointer, Reference, Distance>
{
public:
    using iterator_category = output_iterator_tag;
    using value_type = T;
    using difference_type = Distance;
    using pointer = Pointer;
    using reference = Reference;

    reference operator * () const
    {
        return const_cast<Derived *>(static_cast<Derived const *>(this))->ref();
    }

    Derived & operator ++ () // prefix increment
    {
        return *static_cast<Derived *>(this);
    }

    Derived const & operator ++ (int) // postfix increment
    {
        return *static_cast<Derived *>(this);
    }
};

template <typename Derived
        , typename T
        , typename Pointer
        , typename Reference
        , typename Distance>
class iterator_facade<forward_iterator_tag, Derived, T, Pointer, Reference, Distance>
        : public iterator_facade<input_iterator_tag, Derived, T, Pointer, Reference, Distance>
{
public:
    using iterator_category = forward_iterator_tag;
    using reference = Reference;
    using const_reference = reference const;

    reference operator * ()
    {
        return static_cast<Derived *>(this)->ref();
    }

    const_reference operator * () const
    {
        return static_cast<Derived const *>(this)->ref();
    }
};

template <typename Derived
        , typename T
        , typename Pointer
        , typename Reference
        , typename Distance>
class iterator_facade<bidirectional_iterator_tag, Derived, T, Pointer, Reference, Distance>
        : public iterator_facade<forward_iterator_tag, Derived, T, Pointer, Reference, Distance>
{
public:
    using iterator_category = bidirectional_iterator_tag;
    using difference_type  = Distance;

    Derived & operator -- () // prefix decrement
    {
        static_cast<Derived *>(this)->decrement(1);
        return *static_cast<Derived *>(this);
    }

    Derived operator -- (int) // postfix decrement
    {
        Derived r(*static_cast<Derived *>(this));
        static_cast<Derived *>(this)->decrement(1);
        return r;
    }
};

template <typename Derived
        , typename T
        , typename Pointer
        , typename Reference
        , typename Distance>
class iterator_facade<random_access_iterator_tag, Derived, T, Pointer, Reference, Distance>
        : public iterator_facade<bidirectional_iterator_tag, Derived, T, Pointer, Reference, Distance>
{
public:
    using iterator_category = random_access_iterator_tag;
    using difference_type = Distance;
    using reference = Reference;

    bool equals (Derived const & rhs) const
    {
        return static_cast<Derived const *>(this)->compare(rhs) == 0;
    }

    Derived & operator += (difference_type n)
    {
        static_cast<Derived *>(this)->increment(n);
        return *static_cast<Derived *>(this);
    }

    Derived & operator -= (difference_type n)
    {
        static_cast<Derived *>(this)->decrement(n);
        return *static_cast<Derived *>(this);
    }

    reference operator [] (difference_type index)
    {
        return static_cast<Derived *>(this)->subscript(index);
    }

    friend Derived operator + (Derived const & lhs, difference_type n)
    {
        Derived r(lhs);
        r.increment(n);
        return r;
    }

    friend Derived operator + (difference_type n, Derived const & rhs)
    {
        return operator + (rhs, n);
    }

    friend Derived operator - (Derived const & lhs, difference_type n)
    {
        Derived r(lhs);
        r.decrement(n);
        return r;
    }

    friend difference_type operator - (Derived const & lhs, Derived const & rhs)
    {
        return lhs.diff(rhs);
    }

    friend bool operator < (Derived const & lhs, Derived const & rhs)
    {
        return lhs.compare(rhs) < 0;
    }

    friend bool operator > (Derived const & lhs, Derived const & rhs)
    {
        return lhs.compare(rhs) > 0;
    }

    friend bool operator <= (Derived const & lhs, Derived const & rhs)
    {
        return lhs.compare(rhs) <= 0;
    }

    friend bool operator >= (Derived const & lhs, Derived const & rhs)
    {
        return lhs.compare(rhs) >= 0;
    }
};

template <typename T>
class pointer_proxy_iterator : public iterator_facade<
          random_access_iterator_tag
        , pointer_proxy_iterator<T>
        , T, T *, T &>
{
    using base_class = iterator_facade<
              random_access_iterator_tag
            , pointer_proxy_iterator<T>
            , T, T *, T &>;

public:
    using difference_type = typename base_class::difference_type;
    using pointer = typename base_class::pointer;
    using reference = typename base_class::reference;

private:
    T * _p {nullptr};

public:
    pointer_proxy_iterator ()
    {}

    pointer_proxy_iterator (T * p)
        : _p(p)
    {}

    pointer_proxy_iterator (pointer_proxy_iterator const & it)
        : _p(it._p)
    {}

    pointer_proxy_iterator & operator = (pointer_proxy_iterator const & it)
    {
        _p = it._p;
        return *this;
    }

    int compare (pointer_proxy_iterator const & rhs) const
    {
        return static_cast<int>(_p - rhs._p);
    }

    reference ref ()
    {
        return *_p;
    }

    pointer ptr ()
    {
        return _p;
    }

    const reference ref () const
    {
        return *_p;
    }

    const pointer ptr () const
    {
        return _p;
    }

    void increment (difference_type n)
    {
        _p += n;
    }

    void decrement (difference_type n)
    {
        _p -= n;
    }

    reference subscript (difference_type n)
    {
        return *(_p + n);
    }

    difference_type diff (pointer_proxy_iterator const & rhs) const
    {
        return _p - rhs._p;
    }
};

template <typename ForwardIt>
inline ForwardIt next (ForwardIt it, typename iterator_traits<ForwardIt>::difference_type n = 1)
{
    std::advance(it, n);
    return it;
}

template <typename BidirIt>
inline BidirIt prev (BidirIt it, typename iterator_traits<BidirIt>::difference_type n = 1)
{
    std::advance(it, -n);
    return it;
}

} // pfs
