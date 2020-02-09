#pragma once
#include <pfs/iterator.hpp>
#include <pfs/unicode/char.hpp>
#include <stdexcept>

namespace pfs {
namespace unicode {

template <typename CodePointIter>
struct unicode_iterator_traits;

template <typename CodePointIter>
class utf16_iterator : public iterator_facade<bidirectional_iterator_tag
        , utf16_iterator<CodePointIter>
        , char_t
        , char_t * // unused
        , char_t>  // used as reference in 'std::reverse_iterator's operator: reference operator * () const'
{
    CodePointIter _p;

public:
    typedef iterator_facade<bidirectional_iterator_tag
        , utf16_iterator<CodePointIter>
        , char_t
        , char_t *
        , char_t> base_class;

    typedef typename base_class::difference_type difference_type;

private:
    char_t * operator -> () const; // avoid '->' operator

public:
    utf16_iterator ()
        : _p(0)
    {}

    utf16_iterator (CodePointIter p)
        : _p(p)
    {}

    char_t operator * () const
    {
        return decode(_p, 0);
    }

    operator CodePointIter ()
    {
        return _p;
    }

    CodePointIter base () const
    {
        return _p;
    }

protected:
    static char_t::value_type construct_codepoint (uint16_t w1, uint16_t w2)
    {
        if (w2) {
            char_t::value_type r = (w1 & 0x03FF) << 10;
            r |= w2 & 0x03FF;
            r += 0x10000;
            return r;
        }

        return char_t::value_type(w1);
    }

    // Source: https://www.ietf.org/rfc/rfc2781.txt
    //
    // 2.2 Decoding UTF-16
    //==========================================================================
    // 1) If W1 < 0xD800 or W1 > 0xDFFF, the character value U is the value
    // 	  of W1. Terminate.
    //
    // 2) Determine if W1 is between 0xD800 and 0xDBFF. If not, the sequence
    //    is in error and no valid character can be obtained using W1.
    //    Terminate.
    //
    // 3) If there is no W2 (that is, the sequence ends with W1), or if W2
    //    is not between 0xDC00 and 0xDFFF, the sequence is in error.
    //    Terminate.
    //
    // 4) Construct a 20-bit unsigned integer U', taking the 10 low-order
    //    bits of W1 as its 10 high-order bits and the 10 low-order bits of
    //    W2 as its 10 low-order bits.
    //
    // 5) Add 0x10000 to U' to obtain the character value U. Terminate.
    //
    static void advance_forward_safe (CodePointIter & p, CodePointIter end, difference_type & n)
    {
        if (n < 0) {
            difference_type n1 = -n;
            advance_backward_safe(p, end, n1);
            n += n1;
            return;
        }

        CodePointIter prev = p;
        difference_type nprev = n;

        while (n-- && p < end) {
            prev = p;
            nprev = n + 1;

            if (*p < 0xD800 || *p > 0xDFFF) {
                ++p;
            } else if (*p >= 0xD800 && *p <= 0xDBFF
                    && *(p + 1) >= 0xDC00 && *(p + 1) <= 0xDFFF) {
                p += 2;
            } else { // Invalid char
                ++p;
            }
        }

        if (p > end) {
            p = prev;
            n = nprev;
        }
    }

    static void advance_backward_safe (CodePointIter & p, CodePointIter begin, difference_type & n)
    {
        if (n < 0) {
            difference_type n1 = -n;
            advance_forward_safe(p, begin, n1);
            n += n1;
            return;
        }

        CodePointIter prev = p;
        difference_type nprev = n;

        while (n-- && p > begin) {
            prev = p;
            nprev = n + 1;

            if (*(p - 1) < 0xD800 || *(p - 1) > 0xDFFF) {
                --p;
            } else if (*(p - 2) >= 0xD800 && *(p - 2) <= 0xDBFF
                    && *(p - 1) >= 0xDC00 && *(p - 1) <= 0xDFFF) {
                p -= 2;
            } else { // invalid char
                --p;
            }
        }

        if (p < begin) {
            p = prev;
            n = nprev;
        }
    }

    static void advance_forward (CodePointIter & p, difference_type n)
    {
        advance_forward_safe(p, unicode_iterator_traits<CodePointIter>::max(), n);
    }

    static void advance_backward (CodePointIter & p, difference_type n)
    {
        advance_backward_safe(p, unicode_iterator_traits<CodePointIter>::min(), n);
    }

    static char_t decode (CodePointIter const & p, CodePointIter * pnewpos)
    {
        CodePointIter newpos = p;

        uint16_t w1 = 0;
        uint16_t w2 = 0;

        if (*p < 0xD800 || *p > 0xDFFF) {
            w1 = code_point_cast<uint16_t>(*p);
            ++newpos;
        } else if (*p >= 0xD800 && *p <= 0xDBFF
                && *(p + 1) >= 0xDC00 && *(p + 1) <= 0xDFFF) {
            w1 = code_point_cast<uint16_t>(*p);
            w2 = code_point_cast<uint16_t>(*(p + 1));
            newpos += 2;
        } else { // Invalid char
            throw std::range_error("utf16_iterator::decode()");
        }

        char_t::value_type result = construct_codepoint(w1, w2);

        if (pnewpos)
            *pnewpos = newpos;

        return result;
    }

public:
    static void increment (utf16_iterator & it, difference_type)
    {
        advance_forward(it._p, 1);
    }

    static bool equals (utf16_iterator const & it1, utf16_iterator const & it2)
    {
        return it1._p == it2._p;
    }

    static void decrement (utf16_iterator & it, difference_type)
    {
        advance_backward(it._p, 1);
    }

public:
    void advance_safe (CodePointIter end, difference_type & n)
    {
        advance_forward_safe(_p, end, n);
    }

    static void advance (CodePointIter & p, difference_type n)
    {
        advance_forward(p, n);
    }

    /**
     * @brief Decodes UTF-8 octet sequence into Unicode code point.
     *
     * @return Unicode code point.
     */
    static char_t decode (CodePointIter & p)
    {
        return decode(p, & p);
    }

    template <typename BackInsertIt>
    static BackInsertIt encode (char_t uc, BackInsertIt it);

    friend bool operator < (utf16_iterator const & lhs, utf16_iterator const & rhs)
    {
        return lhs._p < rhs._p;
    }

    friend bool operator <= (utf16_iterator const & lhs, utf16_iterator const & rhs)
    {
        return lhs._p <= rhs._p;
    }

    friend bool operator > (utf16_iterator const & lhs, utf16_iterator const & rhs)
    {
        return lhs._p > rhs._p;
    }

    friend bool operator >= (utf16_iterator const & lhs, utf16_iterator const & rhs)
    {
        return lhs._p >= rhs._p;
    }
};

//
// Source: https://www.ietf.org/rfc/rfc2781.txt
//
//   2.1 Encoding UTF-16
//
//   Encoding of a single character from an ISO 10646 character value to
//   UTF-16 proceeds as follows. Let U be the character number, no greater
//   than 0x10FFFF.
//
//   1) If U < 0x10000, encode U as a 16-bit unsigned integer and
//      terminate.
//
//   2) Let U' = U - 0x10000. Because U is less than or equal to 0x10FFFF,
//      U' must be less than or equal to 0xFFFFF. That is, U' can be
//      represented in 20 bits.
//
//   3) Initialize two 16-bit unsigned integers, W1 and W2, to 0xD800 and
//      0xDC00, respectively. These integers each have 10 bits free to
//      encode the character value, for a total of 20 bits.
//
//   4) Assign the 10 high-order bits of the 20-bit U' to the 10 low-order
//      bits of W1 and the 10 low-order bits of U' to the 10 low-order
//      bits of W2. Terminate.
//
//   Graphically, steps 2 through 4 look like:
//   U' = yyyyyyyyyyxxxxxxxxxx
//   W1 = 110110yyyyyyyyyy
//   W2 = 110111xxxxxxxxxx
//
template <typename CodePointIter>
template <typename BackInsertIt>
BackInsertIt utf16_iterator<CodePointIter>::encode (char_t uc, BackInsertIt it)
{
	if (uc.value < 0x10000) {
		*it++ = uint16_t(uc.value);
	} else {
        uint32_t u1 = uc.value - 0x10000;
        uint16_t w1 = 0xD800 | (u1 >> 10);
        uint16_t w2 = 0xDC00 | (u1 & 0x03FF);
        *it++ = w1;
        *it++ = w2;
    }

    return it;
}

}} // pfs::unicode

