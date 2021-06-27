////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 Vladislav Trifochkin
//
// This file is part of [common-lib](https://github.com/semenovf/common-lib) library.
//
// Changelog:
//      ????.??.?? Initial version
//      2020.11.01 Refactored u8_input_iterator
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <pfs/unicode/utf_iterator.hpp>

/* UTF-8
 *
 * Bits Last code point/    Byte 1      Byte 2      Byte 3      Byte 4      Byte 5      Byte 6
 *  7   U+007F              0xxxxxxx
 *  11  U+07FF              110xxxxx    10xxxxxx
 *  16  U+FFFF              1110xxxx    10xxxxxx    10xxxxxx
 *  21  U+1FFFFF            11110xxx    10xxxxxx    10xxxxxx    10xxxxxx
 *  26  U+3FFFFFF           111110xx    10xxxxxx    10xxxxxx    10xxxxxx    10xxxxxx
 *  31  U+7FFFFFFF          1111110x    10xxxxxx    10xxxxxx    10xxxxxx    10xxxxxx    10xxxxxx
 */

namespace pfs {
namespace unicode {

template <typename OctetInputIt, typename BrokenSeqAction = ignore_broken_sequence>
class u8_input_iterator
    : public details::utf_input_iterator<u8_input_iterator<OctetInputIt, BrokenSeqAction>
        , OctetInputIt, BrokenSeqAction>
{
    using base_class = details::utf_input_iterator<u8_input_iterator, OctetInputIt, BrokenSeqAction>;

public:
    using difference_type = typename base_class::difference_type;

public:
    using base_class::base_class;

    void increment (difference_type)
    {
        if (! this->_p)
            return;

        if (*this->_p == this->_last) {
            this->_p = nullptr;
            this->_flag |= base_class::ATEND_FLAG;
            return;
        }

        uint8_t b = code_point_cast<uint8_t>(**this->_p);
        ++(*this->_p);
        char_t::value_type result;
        int nunits = 0;

        if (b < 128) {
            result = b;
            nunits = 1;
        } else if ((b & 0xE0) == 0xC0) {
            result = b & 0x1F;
            nunits = 2;
        } else if ((b & 0xF0) == 0xE0) {
            result = b & 0x0F;
            nunits = 3;
        } else if ((b & 0xF8) == 0xF0) {
            result = b & 0x07;
            nunits = 4;
        } else if ((b & 0xFC) == 0xF8) {
            result = b & 0x03;
            nunits = 5;
        } else if ((b & 0xFE) == 0xFC) {
            result = b & 0x01;
            nunits = 6;
        } else {
            base_class::broken_sequence();
            return;
        }

        while (--nunits) {
            if (*this->_p == this->_last) {
                base_class::broken_sequence();
                return;
            }

            b = code_point_cast<uint8_t>(**this->_p);
            ++(*this->_p);

            if ((b & 0xC0) == 0x80) {
                result = (result << 6) | (b & 0x3F);
            } else {
                base_class::broken_sequence();
                return;
            }
        }

        this->_value = static_cast<intmax_t>(result);
    }
};

// template <typename OctetInputIt, typename BrokenSeqAction = ignore_broken_sequence>
// class u8_input_iterator : public iterator_facade<input_iterator_tag
//         , u8_input_iterator<OctetInputIt>
//         , char_t
//         , char_t *  // unused
//         , char_t &> // unused
// {
//     typedef iterator_facade<input_iterator_tag
//         , u8_input_iterator<OctetInputIt>
//         , char_t
//         , char_t *
//         , char_t &> base_class;
//
// public:
//     typedef typename base_class::pointer         pointer;
//     typedef typename base_class::reference       reference;
//     typedef typename base_class::difference_type difference_type;
//     typedef BrokenSeqAction broken_sequence_action;
//
// private:
//     static int8_t const ATEND_FLAG  = 0x01;
//     static int8_t const BROKEN_FLAG = 0x02;
//
//     OctetInputIt * _p;
//     OctetInputIt _last;
//     char_t       _value;
//     int8_t       _flag;
//
// public:
//     u8_input_iterator ()
//         : _p(0)
//         , _flag(0)
//     {}
//
//     u8_input_iterator (OctetInputIt & first, OctetInputIt last)
//         : _p(& first)
//         , _last(last)
//         , _flag(0)
//     {
//         if (*_p == _last) {
//             _flag |= ATEND_FLAG;
//         } else {
//             increment(1);
//             _flag |= (_flag & BROKEN_FLAG) ? ATEND_FLAG : 0;
//         }
//     }
//
//     u8_input_iterator (OctetInputIt last)
//         : _p(0)
//         , _last(last)
//         , _flag(ATEND_FLAG)
//     {}
//
// public:
//     reference ref ()
//     {
//         return _value;
//     }
//
//     pointer ptr () const
//     {
//         return & _value;
//     }
//
//     void increment (difference_type);
//
//     bool equals (u8_input_iterator const & rhs) const
//     {
//         return ((_flag & ATEND_FLAG) && (rhs._flag & ATEND_FLAG))
//                 && (_p == rhs._p);
//     }
//
// private:
//     void broken_sequence ()
//     {
//         // Broken utf-8 sequence
//         _value = char_t::replacement_char;
//         _p = 0;
//         _flag |= (ATEND_FLAG | BROKEN_FLAG);
//         broken_sequence_action()();
//     }
// };

template <typename OctetOutputIt>
struct u8_output_iterator_proxy
{
    OctetOutputIt _p;

    u8_output_iterator_proxy (OctetOutputIt p) : _p(p) {}
    void operator = (unicode::char_t const & ch);
};

template <typename OctetOutputIt>
class u8_output_iterator : public iterator_facade<output_iterator_tag
        , u8_output_iterator<OctetOutputIt>
        , void
        , void
        , u8_output_iterator_proxy<OctetOutputIt> >
{
    typedef u8_output_iterator_proxy<OctetOutputIt> proxy_type;
    typedef iterator_facade<output_iterator_tag
        , u8_output_iterator<OctetOutputIt>
        , char_t
        , void
        , proxy_type> base_class;

public:
    typedef typename base_class::pointer         pointer;
    typedef typename base_class::reference       reference;
    typedef typename base_class::difference_type difference_type;

private:
    OctetOutputIt _p;

public:
    u8_output_iterator (OctetOutputIt first)
        : _p(first)
    {}

    reference ref ()
    {
        return proxy_type(_p);
    }
};

template <typename OctetOutputIt>
void u8_output_iterator_proxy<OctetOutputIt>::operator = (unicode::char_t const & ch)
{
    if (!unicode::is_valid(ch))
        return;

    if (ch.value < 0x80) {
        *_p++ = uint8_t(ch.value);
    } else if (ch.value < 0x0800) {
        *_p++ = 0xC0 | uint8_t(ch.value >> 6);
        *_p++ = 0x80 | uint8_t(ch.value & 0x3f);
    } else if (ch.value < 0x10000) {
        *_p++ = 0xE0 | uint8_t(ch.value >> 12);
        *_p++ = 0x80 | (uint8_t(ch.value >> 6)  & 0x3F);
        *_p++ = 0x80 | uint8_t(ch.value & 0x3F);
    } else if (ch.value < 0x200000) {
        *_p++ = 0xF0 | uint8_t(ch.value >> 18);
        *_p++ = 0x80 | (uint8_t(ch.value >> 12) & 0x3F);
        *_p++ = 0x80 | (uint8_t(ch.value >> 6)  & 0x3F);
        *_p++ = 0x80 | uint8_t(ch.value & 0x3F);
    } else if (ch.value < 0x4000000) {
        *_p++ = 0xF8 | uint8_t(ch.value >> 24);
        *_p++ = 0x80 | (uint8_t(ch.value >> 18) & 0x3F);
        *_p++ = 0x80 | (uint8_t(ch.value >> 12) & 0x3F);
        *_p++ = 0x80 | (uint8_t(ch.value >> 6)  & 0x3F);
        *_p++ = 0x80 | uint8_t(ch.value & 0x3F);
    } else if (ch.value < 0x80000000) {
        *_p++ = 0xFC | uint8_t(ch.value >> 30);
        *_p++ = 0x80 | (uint8_t(ch.value >> 24) & 0x3F);
        *_p++ = 0x80 | (uint8_t(ch.value >> 18) & 0x3F);
        *_p++ = 0x80 | (uint8_t(ch.value >> 12) & 0x3F);
        *_p++ = 0x80 | (uint8_t(ch.value >> 6)  & 0x3F);
        *_p++ = 0x80 | uint8_t(ch.value & 0x3F);
    }
}

// TODO OBSOLETE Must be reimplemented (replacing u8_input_iterator)
template <typename OctetInputIt>
class utf8_iterator : public iterator_facade<bidirectional_iterator_tag
        , utf8_iterator<OctetInputIt>
        , char_t
        , char_t *  // unused
        , char_t>   // used as reference in 'std::reverse_iterator's operator: reference operator * () const'
{
    OctetInputIt _p;

public:
    typedef iterator_facade<bidirectional_iterator_tag
        , utf8_iterator<OctetInputIt>
        , char_t
        , char_t *
        , char_t> base_class;

    typedef typename base_class::difference_type difference_type;

private:
    char_t * operator -> () const; // avoid '->' operator

public:
    // It is no matter if _p (when it is a regular pointer)
    // will be initialized with 0 (zero) or uninitialized
    utf8_iterator ()
    {}

    utf8_iterator (OctetInputIt p)
        : _p(p)
    {}

    char_t operator * () const
    {
        return decode(_p, 0);
    }

    operator OctetInputIt ()
    {
        return _p;
    }

    OctetInputIt base () const
    {
        return _p;
    }

protected:
    static void advance_forward_safe (OctetInputIt & p, OctetInputIt end, difference_type & n)
    {
        if (n < 0) {
            difference_type n1 = -n;
            advance_backward_safe(p, end, n1);
            n += n1;
            return;
        }

        OctetInputIt prev = p;
        difference_type nprev = n;

        while (n-- && p < end) {
            prev = p;
            nprev = n + 1;

            if ((*p & 0x80) == 0x00) {
                ++p;
            } else if ((*p & 0xE0) == 0xC0) {
                p += 2;
            } else if ((*p & 0xF0) == 0xE0) {
                p += 3;
            } else if ((*p & 0xF8) == 0xF0) {
                p += 4;
            } else if ((*p & 0xFC) == 0xF8) {
                p += 5;
            } else if ((*p & 0xFE) == 0xFC) {
                p += 6;
            } else { // Invalid char
                ++p;
            }
        }

        if (p > end) {
            p = prev;
            n = nprev;
        }
    }

    static void advance_backward_safe (OctetInputIt & p, OctetInputIt begin, difference_type & n)
    {
        if (n < 0) {
            difference_type n1 = -n;
            advance_forward_safe(p, begin, n1);
            n += n1;
            return;
        }

        OctetInputIt prev = p;
        difference_type nprev = n;

        while (n-- && p > begin) {
            prev = p;
            nprev = n + 1;

            if ((*(p - 1) & 0x80) == 0x00) {
                --p;
            } else if ((*(p - 2) & 0xE0) == 0xC0) {
                p -= 2;
            } else if ((*(p - 3) & 0xF0) == 0xE0) {
                p -= 3;
            } else if ((*(p - 4) & 0xF8) == 0xF0) {
                p -= 4;
            } else if ((*(p - 5) & 0xFC) == 0xF8) {
                p -= 5;
            } else if ((*(p - 6) & 0xFE) == 0xFC) {
                p -= 6;
            } else {
                --p;
            }
        }

        if (p < begin) {
            p = prev;
            n = nprev;
        }
    }

    static void advance_forward (OctetInputIt & p, difference_type n)
    {
        advance_forward_safe(p, unicode_iterator_traits<OctetInputIt>::max(), n);
    }

    static void advance_backward (OctetInputIt & p, difference_type n)
    {
        advance_backward_safe(p, unicode_iterator_traits<OctetInputIt>::min(), n);
    }

    static char_t decode (OctetInputIt const & p, OctetInputIt * pnewpos)
    {
        OctetInputIt newpos = p;
        uint8_t b = code_point_cast<uint8_t> (*newpos);
        char_t::value_type result;
        int nunits = 0;

        if (b < 128) {
            result = b;
            nunits = 1;
        } else if ((b & 0xE0) == 0xC0) {
            result = b & 0x1F;
            nunits = 2;
        } else if ((b & 0xF0) == 0xE0) {
            result = b & 0x0F;
            nunits = 3;
        } else if ((b & 0xF8) == 0xF0) {
            result = b & 0x07;
            nunits = 4;
        } else if ((b & 0xFC) == 0xF8) {
            result = b & 0x03;
            nunits = 5;
        } else if ((b & 0xFE) == 0xFC) {
            result = b & 0x01;
            nunits = 6;
        } else {
            // Invalid
            throw std::range_error("utf8_iterator::decode()");
        }

        ++newpos;

        while (--nunits) {
            b = code_point_cast<uint8_t>(*newpos);

            if ((b & 0xC0) == 0x80) {
                result = (result << 6) | (b & 0x3F);
            } else {
                // Invalid
                throw std::range_error("utf8_iterator::decode()");
            }
            ++newpos;
        }

        if (pnewpos)
            *pnewpos = newpos;

        return result;
    }

public:
    static void increment (utf8_iterator & it, difference_type)
    {
        advance_forward(it._p, 1);
    }

    static bool equals (utf8_iterator const & it1, utf8_iterator const & it2)
    {
        return it1._p == it2._p;
    }

    static void decrement (utf8_iterator & it, difference_type)
    {
        advance_backward(it._p, 1);
    }

public:
    void advance_safe (OctetInputIt end, difference_type & n)
    {
        advance_forward_safe(_p, end, n);
    }

    static void advance (OctetInputIt & p, difference_type n)
    {
        advance_forward(p, n);
    }

    /**
     * @brief Decodes UTF-8 octet sequence into Unicode code point.
     *
     * @return Unicode code point.
     */
    static char_t decode (OctetInputIt & p)
    {
        return decode(p, & p);
    }

    template <typename BackInsertIt>
    static BackInsertIt encode (char_t uc, BackInsertIt it);

    friend bool operator < (utf8_iterator const & lhs, utf8_iterator const & rhs)
    {
        return lhs._p < rhs._p;
    }

    friend bool operator <= (utf8_iterator const & lhs, utf8_iterator const & rhs)
    {
        return lhs._p <= rhs._p;
    }

    friend bool operator > (utf8_iterator const & lhs, utf8_iterator const & rhs)
    {
        return lhs._p > rhs._p;
    }

    friend bool operator >= (utf8_iterator const & lhs, utf8_iterator const & rhs)
    {
        return lhs._p >= rhs._p;
    }
};

template <typename OctetInputIt>
template <typename BackInsertIt>
BackInsertIt utf8_iterator<OctetInputIt>::encode (char_t uc, BackInsertIt it)
{
    if (uc.value < 0x80) {
        *it++ = uint8_t(uc.value);
    } else if (uc.value < 0x0800) {
        *it++ = 0xC0 | uint8_t(uc.value >> 6);
        *it++ = 0x80 | uint8_t(uc.value & 0x3f);
    } else if (uc.value < 0x10000) {
        *it++ = 0xE0 | uint8_t(uc.value >> 12);
        *it++ = 0x80 | (uint8_t(uc.value >> 6)  & 0x3F);
        *it++ = 0x80 | uint8_t(uc.value & 0x3F);
    } else if (uc.value < 0x200000) {
        *it++ = 0xF0 | uint8_t(uc.value >> 18);
        *it++ = 0x80 | (uint8_t(uc.value >> 12) & 0x3F);
        *it++ = 0x80 | (uint8_t(uc.value >> 6)  & 0x3F);
        *it++ = 0x80 | uint8_t(uc.value & 0x3F);
    } else if (uc.value < 0x4000000) {
        *it++ = 0xF8 | uint8_t(uc.value >> 24);
        *it++ = 0x80 | (uint8_t(uc.value >> 18) & 0x3F);
        *it++ = 0x80 | (uint8_t(uc.value >> 12) & 0x3F);
        *it++ = 0x80 | (uint8_t(uc.value >> 6)  & 0x3F);
        *it++ = 0x80 | uint8_t(uc.value & 0x3F);
    } else if (uc.value < 0x80000000) {
        *it++ = 0xFC | uint8_t(uc.value >> 30);
        *it++ = 0x80 | (uint8_t(uc.value >> 24) & 0x3F);
        *it++ = 0x80 | (uint8_t(uc.value >> 18) & 0x3F);
        *it++ = 0x80 | (uint8_t(uc.value >> 12) & 0x3F);
        *it++ = 0x80 | (uint8_t(uc.value >> 6)  & 0x3F);
        *it++ = 0x80 | uint8_t(uc.value & 0x3F);
    }

    return it;
}

}} // pfs::unicode

// namespace pfs {
//
// template <typename StringType, typename OctetInputIt>
// StringType read_line_u8 (OctetInputIt & first, OctetInputIt last)
// {
//     typedef unicode::u8_input_iterator<OctetInputIt>  input_iterator;
//     typedef unicode::u8_output_iterator<pfs::back_insert_iterator<StringType> > output_iterator;
//
//     StringType result;
//     output_iterator out(pfs::back_inserter(result));
//     input_iterator it(first, last);
//     input_iterator end(last);
//
//     while (it != end && *it != '\n') {
//         if (*it != '\r')
//             *out++ = *it;
//         ++it;
//     }
//
//     return result;
// }
//
// template <typename StringType, typename OctetInputIt>
// StringType read_all_u8 (OctetInputIt & first, OctetInputIt last)
// {
//     typedef unicode::u8_input_iterator<OctetInputIt>  input_iterator;
//     typedef unicode::u8_output_iterator<pfs::back_insert_iterator<StringType> > output_iterator;
//
//     StringType result;
//     output_iterator out(pfs::back_inserter(result));
//
//     input_iterator it(first, last);
//     input_iterator end(last);
//
//     while (it != end) {
//         if (*it != '\r')
//             *out++ = *it;
//         ++it;
//     }
//
//     return result;
// }
//
// } // pfs
