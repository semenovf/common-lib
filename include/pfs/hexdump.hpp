////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2025.07.07 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "namespace.hpp"
#include "fmt.hpp"
#include <iterator>
#include <cctype>

PFS__NAMESPACE_BEGIN

/**
 * Hex dump of bytes.
 *
 * @param first Pointer to first byte.
 * @param last Pointer to byte after last.
 * @param bytes_per_line Number of hex and ASCII encoded bytes per line.
 * @return Hex dump.
 */
template <typename OctetForwardIterator>
std::string hexdump (OctetForwardIterator first, OctetForwardIterator last, std::size_t bytes_per_line = 16)
{
    // constexpr std::size_t offset_field_size = 10;

    std::size_t offset = 0;
    // std::size_t reserve_line_size = offset_field_size + 4 * bytes_per_line;
    // std::size_t reserve_size = reserve_line_size * (1 + size / bytes_per_line) ;
    std::string result;

    // result.reserve(reserve_size);
    auto pos = first;

    std::string hex;
    std::string ascii;

    while (pos != last) {
        hex.clear();
        ascii.clear();

        std::size_t j = 0;

        for (; j < bytes_per_line && pos != last; j++, ++pos) {
            auto ch = static_cast<char>(*pos);

            hex += fmt::format("{:02X} ", ch);
            ascii += std::isprint(ch) ? ch : '.';
        }

        if (j < bytes_per_line) {
            j = bytes_per_line - j;

            while (j-- > 0)
                hex += "   ";
        }

        result += fmt::format("{:08X}  {} {}\n", offset, hex, ascii);
        offset += bytes_per_line;
    }

    return result;
}

PFS__NAMESPACE_END
