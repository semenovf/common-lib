////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2020.11.01 Initial version
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <pfs/iterator.hpp>
#include <pfs/unicode/char.hpp>
#include <pfs/unicode/traits.hpp>

namespace pfs {
namespace unicode {

template <typename CodePointIter>
struct unicode_iterator_traits;

namespace details {

template <typename Derived, typename XtetInputIt, typename BrokenSeqAction>
class utf_input_iterator : public iterator_facade<input_iterator_tag
        , utf_input_iterator<Derived, XtetInputIt, BrokenSeqAction>
        , char_t
        , char_t *  // unused
        , char_t &> // unused
{
    typedef iterator_facade<input_iterator_tag
        , utf_input_iterator<Derived, XtetInputIt, BrokenSeqAction>
        , char_t
        , char_t *
        , char_t &> base_class;

public:
    typedef typename base_class::pointer         pointer;
    typedef typename base_class::reference       reference;
    typedef typename base_class::difference_type difference_type;
    typedef BrokenSeqAction broken_sequence_action;

protected:
    static int8_t constexpr ATEND_FLAG  = 0x01;
    static int8_t constexpr BROKEN_FLAG = 0x02;

    XtetInputIt * _p {nullptr};
    XtetInputIt  _last;
    char_t       _value;
    int8_t       _flag;

public:
    utf_input_iterator ()
        : _p(0)
        , _flag(0)
    {}

    utf_input_iterator (XtetInputIt & first, XtetInputIt last)
        : _p(& first)
        , _last(last)
        , _flag(0)
    {
        if (*_p == _last) {
            _flag |= ATEND_FLAG;
        } else {
            increment(1);
            _flag |= (_flag & BROKEN_FLAG) ? ATEND_FLAG : 0;
        }
    }

    utf_input_iterator (XtetInputIt last)
        : _p(0)
        , _last(last)
        , _flag(ATEND_FLAG)
    {}

public:
    reference ref ()
    {
        return _value;
    }

    pointer ptr () const
    {
        return & _value;
    }

    bool equals (utf_input_iterator const & rhs) const
    {
        return ((_flag & ATEND_FLAG) && (rhs._flag & ATEND_FLAG))
                && (_p == rhs._p);
    }

    void increment (difference_type)
    {
        static_cast<Derived *>(this)->increment(1);
    }

    bool is_broken () const
    {
        return _flag & BROKEN_FLAG;
    }

protected:
    void broken_sequence ()
    {
        // Broken utf-8 sequence
        _value = char_t::replacement_char;
        _p = 0;
        _flag |= (ATEND_FLAG | BROKEN_FLAG);
        broken_sequence_action()();
    }
};

} // details

}} // pfs::unicode
