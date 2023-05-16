////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2020.11.01 Initial version
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "utf_iterator.hpp"
#include "pfs/endian.hpp"

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

struct utf16le_byteswap
{
    static std::uint16_t byteswap (std::uint16_t x)
    {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        return pfs::byteswap(x);
#else
        return x;
#endif
    }
};

struct utf16be_byteswap
{
    static std::uint16_t byteswap (std::uint16_t x)
    {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        return pfs::byteswap(x);
#else
        return x;
#endif
    }
};

template <typename HextetFwdIt, typename Utf18ByteSwap>
class utf16_iterator
    : public details::utf_iterator<utf16_iterator<HextetFwdIt, Utf18ByteSwap>, HextetFwdIt>
{
    using base_class = details::utf_iterator<utf16_iterator, HextetFwdIt>;

    friend class details::utf_iterator<utf16_iterator, HextetFwdIt>;

public:
    using difference_type = typename base_class::difference_type;

protected:
    char_t advance (HextetFwdIt & pos, HextetFwdIt last, difference_type n) const
    {
        char_t::value_type result;

        while (n-- > 0) {
            if (pos == last)
                throw error {make_error_code(errc::broken_sequence)};

            std::uint16_t w1 = code_unit_cast<std::uint16_t>(*pos);
            w1 = Utf18ByteSwap::byteswap(w1);

            ++pos;

            std::uint16_t w2 = 0;

            // Valid unit
            if (w1 < 0xD800 || w1 > 0xDFFF) {
                ;
            } else if (w1 >= 0xD800 && w1 <= 0xDBFF) {
                if (pos == last)
                    throw error {make_error_code(errc::broken_sequence)};

                w2 = code_unit_cast<std::uint16_t>(*pos);
                w2 = Utf18ByteSwap::byteswap(w2);

                // Valid unit sequence
                if (w2 >= 0xDC00 && w2 <= 0xDFFF)
                    ++pos;
                else
                    throw error {make_error_code(errc::broken_sequence)};
            } else {
                throw error {make_error_code(errc::broken_sequence)};
            }

            if (w2) {
                result = (w1 & 0x03FF) << 10;
                result |= w2 & 0x03FF;
                result += 0x10000;
            } else {
                result = w1;
            }
        }

        return char_t{result};
    }

    /**
     * Advance iterator @a pos by @a n code points.
     */
    void advance (HextetFwdIt & pos, difference_type n) const
    {
        while (n--) {
            std::uint16_t w1 = code_unit_cast<std::uint16_t>(*pos);
            w1 = Utf18ByteSwap::byteswap(w1);
            ++pos;

            if (w1 < 0xD800 || w1 > 0xDFFF) {
                ;
            } else if (w1 >= 0xD800 && w1 <= 0xDBFF) {
                ++pos;
            } else {
                throw error {make_error_code(errc::broken_sequence)};
            }
        }
    }

    /**
     * Returns distance in a code points (first member in the pair) and in a
     * code units (seconds member in the pair) bitween @a pos and @a last position.
     *
     * @note This implementation does not check sequence consistency and
     *       is assumed that @a pos is before @a last.
     */
    std::pair<difference_type, difference_type> distance (HextetFwdIt pos
        , HextetFwdIt last) const
    {
        difference_type cp_count = 0;
        difference_type cu_count = 0;

        while (pos != last) {
            std::uint16_t w1 = code_unit_cast<std::uint16_t>(*pos);
            w1 = Utf18ByteSwap::byteswap(w1);
            ++pos;

            if (w1 < 0xD800 || w1 > 0xDFFF) {
                cu_count++;
            } else if (w1 >= 0xD800 && w1 <= 0xDBFF) {
                cu_count += 2;
                ++pos;
            } else {
                throw error {make_error_code(errc::broken_sequence)};
            }

            cp_count++;
        }

        return std::make_pair(cp_count, cu_count);
    }

public:
    using base_class::base_class;
};

template <typename HextetFwdIt>
using utf16le_iterator = utf16_iterator<HextetFwdIt, utf16le_byteswap>;

template <typename HextetFwdIt>
using utf16be_iterator = utf16_iterator<HextetFwdIt, utf16be_byteswap>;

}} // pfs::unicode
