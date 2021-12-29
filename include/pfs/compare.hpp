////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2019.??.?? Initial version
////////////////////////////////////////////////////////////////////////////////
#pragma once

namespace pfs {

// Base class (used by inheritor for ADL)
struct compare_operations {};

template <typename T, typename U>
inline bool operator != (T const & a, U const & b)
{
    return !(a == b);
}

template <typename T, typename U>
inline bool operator <= (T const & a, U const & b)
{
    return !(b < a);
}

template <typename T, typename U>
inline bool operator > (T const & a, U const & b)
{
    return b < a;
}

template <typename T, typename U>
inline bool operator >= (T const & a, U const & b)
{
    return !(a < b);
}

} // namespace pfs
