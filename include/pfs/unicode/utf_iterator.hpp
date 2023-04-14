////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2020.11.01 Initial version
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "pfs/error.hpp"
#include "pfs/iterator.hpp"
#include "char.hpp"
#include "traits.hpp"

namespace pfs {
namespace unicode {

template <typename CodePointIter>
struct unicode_iterator_traits;

namespace details {

template <typename Derived, typename XtetInputIt>
class utf_input_iterator : public iterator_facade<input_iterator_tag
        , utf_input_iterator<Derived, XtetInputIt>
        , char_t
        , char_t *  // unused
        , char_t &> // unused
{
    typedef iterator_facade<input_iterator_tag
        , utf_input_iterator<Derived, XtetInputIt>
        , char_t
        , char_t *
        , char_t &> base_class;

public:
    typedef typename base_class::pointer         pointer;
    typedef typename base_class::reference       reference;
    typedef typename base_class::difference_type difference_type;

protected:
    XtetInputIt _p;
    XtetInputIt _next;
    XtetInputIt _last;
    char_t      _value;

protected:
    utf_input_iterator (XtetInputIt first, XtetInputIt last)
        : _p(first)
        , _next(first)
        , _last(last)
    {}

    explicit utf_input_iterator (XtetInputIt last)
        : _p(last)
        , _next(last)
        , _last(last)
    {}

public:
    static Derived begin (XtetInputIt first, XtetInputIt last)
    {
        return Derived {first, last};
    }

    static Derived end (XtetInputIt last)
    {
        return Derived {last};
    }

public:
    reference ref ()
    {
        if (_p == _next)
            _value = static_cast<Derived *>(this)->advance(_next, _last, 1);

        return _value;
    }

    pointer ptr () const
    {
        if (_p == _next)
            _value = static_cast<Derived *>(this)->advance(_next, _last, 1);

        return & _value;
    }

    bool equals (utf_input_iterator const & rhs) const
    {
        return _p == rhs._p;
    }

    void increment (difference_type)
    {
        static_cast<Derived *>(this)->increment(1);
        _next = _p;
    }

    Derived end () const noexcept
    {
        return Derived {_last};
    }

};

} // details

}} // pfs::unicode
