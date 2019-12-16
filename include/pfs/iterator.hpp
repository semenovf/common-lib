////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 Vladislav Trifochkin
//
// This file is part of [pfs-common](https://github.com/semenovf/pfs-common) library.
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

template <typename Category, typename T, typename Distance = ptrdiff_t,
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
    using difference_type = ptrdiff_t;
    using pointer = T *;
    using reference = T &;
};

template <typename T>
struct iterator_traits<T const *>
{
    typedef std::random_access_iterator_tag iterator_category;
    typedef T         value_type;
    typedef ptrdiff_t difference_type;
    typedef const T * pointer;
    typedef const T & reference;
};

template <typename Category
        , typename Derived
        , typename T
        , typename Pointer
        , typename Reference
        , typename Distance = ptrdiff_t>
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
struct iterator_facade<random_access_iterator_tag, Derived, T, Pointer, Reference, Distance>
        : public iterator_facade<bidirectional_iterator_tag, Derived, T, Pointer, Reference, Distance>
{
    typedef random_access_iterator_tag iterator_category;
    typedef Distance     difference_type;
    typedef Reference    reference;

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

// template <typename InputIt>
// inline typename pfs::iterator_traits<InputIt>::difference_type
// distance (InputIt begin, InputIt end)
// {
//     return std::distance(begin, end);
// }
//
// template <typename InputIt>
// inline void
// advance (InputIt & it, typename pfs::iterator_traits<InputIt>::difference_type n)
// {
//     return std::advance(it, n);
// }

#if __cplusplus < 201103L

// template <typename Iterator>
// struct reverse_iterator : public std::reverse_iterator<Iterator>
// {
//     typedef std::reverse_iterator<Iterator> base_class;
//
//     reverse_iterator () : base_class() {}
//
//     explicit reverse_iterator (Iterator x)
//         : base_class(x)
//     {}
//
//     template <typename U>
//     reverse_iterator (reverse_iterator<U> const & other)
//         : base_class(other)
//     {}
// };

//
// C++98 (GCC) implementation need `const_reference` type defined in Container
// for `back_insert_iterator & operator=(typename _Container::const_reference __value)`.
// But pfs::string has no such typedef, so error occurred.
// So replicate this struct.
//
// template <typename Container>
// class back_insert_iterator : public std::iterator<std::output_iterator_tag, void, void, void, void>
// {
// protected:
//     Container * _container;
//
// public:
//     typedef Container container_type;
//
//     explicit back_insert_iterator (Container & x)
//         : _container(pfs::addressof(x))
//     {}
//
//     back_insert_iterator &
//             operator = (const typename Container::value_type & value)
//     {
//         _container->push_back(value);
//         return *this;
//     }
//
//     back_insert_iterator & operator * ()
//     {
//         return *this;
//     }
//
//     back_insert_iterator & operator ++ ()
//     {
//         return *this;
//     }
//
//     back_insert_iterator operator ++ (int)
//     {
//         return *this;
//     }
// };
//
// template <typename Container>
// inline back_insert_iterator<Container>
// back_inserter (Container & c)
// {
//     return back_insert_iterator<Container>(c);
// }

#endif

template <typename OStream>
struct ostream_iterator
{
    typedef typename OStream::char_type char_type;

    ostream_iterator (OStream & os)
        : _pos(& os)
    {}

    ostream_iterator & operator = (char_type const & value)
    {
        *_pos << value;
        return *this;
    }

    ostream_iterator & operator * ()
    {
        return *this;
    }

    ostream_iterator & operator ++ ()
    {
        return *this;
    }

    ostream_iterator const & operator ++ (int)
    {
        return *this;
    }

private:
    OStream * _pos;
};


template <typename CharT, typename Traits = std::char_traits<CharT> >
class istreambuf_iterator : public std::istreambuf_iterator<CharT, Traits>
{
    typedef std::istreambuf_iterator<CharT, Traits> base_class;
public:
    constexpr istreambuf_iterator () noexcept
        : base_class()
    {}

    istreambuf_iterator (std::basic_istream<CharT,Traits> & is) noexcept
        : base_class(is)
    {}

    istreambuf_iterator (std::basic_streambuf<CharT,Traits> * is) noexcept
        : base_class(is)
    {}
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
