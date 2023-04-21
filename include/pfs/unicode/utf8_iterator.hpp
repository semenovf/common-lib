////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2023 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      ????.??.?? Initial version
//      2020.11.01 Refactored u8_input_iterator
//      2023.04.13 Added `advance` method.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "utf_iterator.hpp"

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

//////////////////////////////////////////////////////////////////////////////////////////
//
// ATTENTION!
// It's not recommended to use with single-pass input iterators (i.e.
// std::istreambuf_iterator). Permitted operations are very limited (and they
// change state of the underlying iterator):
//      * prefixed increment operator (++it)
//      * std::distance()
//
//////////////////////////////////////////////////////////////////////////////////////////

template <typename OctetInputIt>
class utf8_input_iterator
    : public details::utf_input_iterator<utf8_input_iterator<OctetInputIt>, OctetInputIt>
{
    using base_class = details::utf_input_iterator<utf8_input_iterator, OctetInputIt>;

    friend class details::utf_input_iterator<utf8_input_iterator, OctetInputIt>;

public:
    using difference_type = typename base_class::difference_type;

protected:
    char_t advance (OctetInputIt & pos, OctetInputIt last, difference_type n)
    {
        char_t::value_type result;

        while (n-- > 0) {
            if (pos == last)
                throw error {make_error_code(errc::broken_sequence)};

            std::uint8_t b = code_unit_cast<std::uint8_t>(*pos);
            ++pos;
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
                throw error {make_error_code(errc::broken_sequence)};
            }

            while (--nunits) {
                if (pos == last)
                    throw error {make_error_code(errc::broken_sequence)};

                b = code_unit_cast<uint8_t>(*pos);
                ++pos;

                if ((b & 0xC0) == 0x80) {
                    result = (result << 6) | (b & 0x3F);
                } else {
                    throw error {make_error_code(errc::broken_sequence)};
                }
            }

            result = static_cast<intmax_t>(result);
        }

        return result;
    }

public:
    using base_class::base_class;

    /**
     * Returns distance in a code points (first member in the pair) and in a
     * code units (seconds member in the pair) bitween @a pos and @a last position.
     *
     * @note This implementation does not check sequence consistency and
     *       is assumed that @a pos is before @a last.
     */
    static std::pair<difference_type, difference_type> distance_unsafe (OctetInputIt pos
        , OctetInputIt last)
    {
        difference_type cp_count = 0;
        difference_type cu_count = 0;

        while (pos != last) {
            std::uint8_t b = code_unit_cast<std::uint8_t>(*pos);
            ++pos;

            if (b < 128) {
                cu_count++;
            } else if ((b & 0xE0) == 0xC0) {
                cu_count += 2;
                ++pos;
            } else if ((b & 0xF0) == 0xE0) {
                cu_count += 3;
                pos += 2;
            } else if ((b & 0xF8) == 0xF0) {
                cu_count += 4;
                pos += 3;
            } else if ((b & 0xFC) == 0xF8) {
                cu_count += 5;
                pos += 4;
            } else if ((b & 0xFE) == 0xFC) {
                cu_count += 6;
                pos += 5;
            } else {
                throw error {make_error_code(errc::broken_sequence)};
            }

            cp_count++;
        }

        return std::make_pair(cp_count, cu_count);
    }
};

template <typename OctetOutputIt>
struct utf8_output_iterator_proxy
{
    OctetOutputIt _p;

    utf8_output_iterator_proxy (OctetOutputIt p) : _p(p) {}
    void operator = (unicode::char_t const & ch);
};

template <typename OctetOutputIt>
class utf8_output_iterator : public iterator_facade<output_iterator_tag
        , utf8_output_iterator<OctetOutputIt>
        , void
        , void
        , utf8_output_iterator_proxy<OctetOutputIt> >
{
    typedef utf8_output_iterator_proxy<OctetOutputIt> proxy_type;
    typedef iterator_facade<output_iterator_tag
        , utf8_output_iterator<OctetOutputIt>
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
    utf8_output_iterator (OctetOutputIt first)
        : _p(first)
    {}

    reference ref ()
    {
        return proxy_type(_p);
    }
};

template <typename OctetOutputIt>
void utf8_output_iterator_proxy<OctetOutputIt>::operator = (unicode::char_t const & ch)
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

}} // pfs::unicode
