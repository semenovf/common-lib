////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020-2023 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2020.11.01 Initial version
//      2023.05.12 Renamed `utf_input_iterator` into `utf_iterator` and changed
//                 category from `input` to `forward` iterator.
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

template <typename Derived, typename XtetFwdIt>
class utf_iterator : public iterator_facade<forward_iterator_tag
        , utf_iterator<Derived, XtetFwdIt>
        , char_t
        , char_t *  // unused
        , char_t &> // unused
{
    typedef iterator_facade<forward_iterator_tag
        , utf_iterator<Derived, XtetFwdIt>
        , char_t
        , char_t *
        , char_t &> base_class;

public:
    typedef typename base_class::pointer         pointer;
    typedef typename base_class::reference       reference;
    typedef typename base_class::difference_type difference_type;

protected:
    XtetFwdIt _p;
    XtetFwdIt _next;
    XtetFwdIt _last;
    char_t      _value;

protected:
    utf_iterator (XtetFwdIt first, XtetFwdIt last)
        : _p(first)
        , _next(first)
        , _last(last)
    {}

    explicit utf_iterator (XtetFwdIt last)
        : _p(last)
        , _next(last)
        , _last(last)
    {}

public:
    static Derived begin (XtetFwdIt first, XtetFwdIt last)
    {
        return Derived {first, last};
    }

    static Derived end (XtetFwdIt last)
    {
        return Derived {last};
    }

public:
    // Input iterator requirements
    reference ref ()
    {
        if (_p == _next)
            _value = static_cast<Derived *>(this)->advance(_next, _last, 1);

        return _value;
    }

    // Input iterator requirements
    pointer ptr () const
    {
        if (_p == _next)
            _value = static_cast<Derived *>(this)->advance(_next, _last, 1);

        return & _value;
    }

    // Input iterator requirements
    bool equals (utf_iterator const & rhs) const
    {
        return _p == rhs._p;
    }

    // Input iterator requirements
    void increment (difference_type n)
    {
        static_cast<Derived *>(this)->advance(this->_p, this->_last, n);
        _next = _p;
    }

    Derived end () const noexcept
    {
        return Derived {_last};
    }

    XtetFwdIt base () const noexcept
    {
        return _p;
    }

public:
    /**
     * Returns distance in a code points (first member in the pair) and in a
     * code units (seconds member in the pair) bitween @a pos and @a last position.
     *
     * @note This implementation does not check sequence consistency and
     *       is assumed that @a pos is before @a last.
     */
    static std::pair<difference_type, difference_type> distance_unsafe (
          Derived const & first, Derived const & last)
    {
        return first.distance(first._p, last._p);
    }

    /**
     * Advance iterator @a pos by @a n code points.
     */
    static void advance_unsafe (Derived & pos, difference_type n)
    {
        XtetFwdIt p = pos._p;
        pos.advance(p, n);
        pos = begin(p, pos._last);
    }
};

} // details

}} // pfs::unicode
