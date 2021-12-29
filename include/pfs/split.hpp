////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2017.01.26 Initial version of pfs/algo/split.hpp
//      2021.07.06 Refactored.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "find.hpp"

namespace pfs {

enum class keep_empty: bool {
      no  = false
    , yes = true
};

/**
 * Splits sequence specified by pair @a begin and @a end into tokens separated
 * by sequence specified by pair @a separator_begin and @a separator_end.
 *
 * @param result output iterator to store tokens.
 * @param begin Begin position of sequence.
 * @param end End position of sequence.
 * @param separator_begin Begin position of separator.
 * @param separator_end End position of separator.
 * @param flag Must be one of @c keep_empty::yes or @c keep_empty::no.
 * @return Output iterator for collection of tokens.
 */
template <typename OutputIt, typename ForwardIt1, typename ForwardIt2>
OutputIt split (OutputIt result
    , ForwardIt1 begin, ForwardIt1 end
    , ForwardIt2 separator_begin, ForwardIt2 separator_end
    , keep_empty flag)
{
    using container_type = typename OutputIt::container_type;
    using value_type = typename container_type::value_type;

    if (begin == end)
        return result;

    while (begin != end) {
        auto r = find(begin, end, separator_begin, separator_end);

        if (r.first == end)
            break;

        if (begin != r.first) {
            // token is not empty - add it
            *result++ = value_type(begin, r.first);
        } else if (flag == keep_empty::yes) {
            // token is empty but need to keep empty - add it
            *result++ = value_type{};
        }

        // Skip separator
        begin = r.second;
    }

    if (begin != end) {
        *result++ = value_type(begin, end);
    } else {
        if (flag == keep_empty::yes)
            *result++ = value_type{};
    }

    return result;
}

} // namespace pfs
