////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of [pfs-common](https://github.com/semenovf/pfs-common) library.
//
// Changelog:
//      2020.11.01 Initial version
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <pfs/unicode/utf_iterator.hpp>

/*
 * Source: https://www.ietf.org/rfc/rfc2781.txt
 *
 * 2.2 Decoding UTF-16
 * =============================================================================
 * 1) If W1 < 0xD800 or W1 > 0xDFFF, the character value U is the value
 *    of W1. Terminate.
 *
 * 2) Determine if W1 is between 0xD800 and 0xDBFF. If not, the sequence
 *    is in error and no valid character can be obtained using W1.
 *    Terminate.
 *
 * 3) If there is no W2 (that is, the sequence ends with W1), or if W2
 *    is not between 0xDC00 and 0xDFFF, the sequence is in error.
 *    Terminate.
 *
 * 4) Construct a 20-bit unsigned integer U', taking the 10 low-order
 *    bits of W1 as its 10 high-order bits and the 10 low-order bits of
 *    W2 as its 10 low-order bits.
 *
 * 5) Add 0x10000 to U' to obtain the character value U. Terminate.
 *
 */

namespace pfs {
namespace unicode {

template <typename HextetInputIt, typename BrokenSeqAction = ignore_broken_sequence>
class u16_input_iterator
    : public details::utf_input_iterator<u16_input_iterator<HextetInputIt, BrokenSeqAction>
        , HextetInputIt, BrokenSeqAction>
{
    using base_class = details::utf_input_iterator<u16_input_iterator
        , HextetInputIt, BrokenSeqAction>;

    bool _little_endian {true};

public:
    using difference_type = typename base_class::difference_type;

public:
    using base_class::base_class;

    u16_input_iterator (bool little_endian)
        : base_class()
        , _little_endian(little_endian)
    {}

    u16_input_iterator (HextetInputIt & first, HextetInputIt last, bool little_endian)
        : base_class(first, last)
        , _little_endian(little_endian)
    {}

    u16_input_iterator (HextetInputIt last, bool little_endian)
        : base_class(last)
        , _little_endian(little_endian)
    {}

    void increment (difference_type)
    {
        if (! this->_p)
            return;

        if (*this->_p == this->_last) {
            this->_p = nullptr;
            this->_flag |= base_class::ATEND_FLAG;
            return;
        }

        uint16_t w1 = code_point_cast<uint16_t>(**this->_p);
        uint16_t w2 = 0;

        ++*this->_p;

        // Valid unit
        if (w1 < 0xD800 || w1 > 0xDFFF) {
            ;
        } else if (w1 >= 0xD800 && w1 <= 0xDBFF) {
            if (*this->_p == this->_last) {
                base_class::broken_sequence();
                return;
            }

            w2 = code_point_cast<uint16_t>(**this->_p);

            // Valid unit sequence
            if (w2 >= 0xDC00 && w2 <= 0xDFFF) {
                ++*this->_p;
            } else {
                // Invalid unit
                base_class::broken_sequence();
                return;
            }
        } else {
            base_class::broken_sequence();
            return;
        }

        // Construct code point
        char_t::value_type result;

        if (w2) {
            result = (w1 & 0x03FF) << 10;
            result |= w2 & 0x03FF;
            result += 0x10000;
        } else {
            result = w1;
        }

        this->_value = static_cast<intmax_t>(result);
    }
};

}} // pfs::unicode
