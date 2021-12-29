////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2020.04.03 Initial version
////////////////////////////////////////////////////////////////////////////////
#pragma once

namespace pfs {

// [How to static_cast std::unique_ptr](https://www.ficksworkshop.com/blog/how-to-static-cast-std-unique-ptr)
// [Alternatives of static_pointer_cast for unique_ptr](https://stackoverflow.com/questions/36120424/alternatives-of-static-pointer-cast-for-unique-ptr)

template <typename T, typename U>
unique_ptr<T> static_unique_pointer_cast (unique_ptr<U> && r)
{
    return unique_ptr<T>{static_cast<T*>(r.release())};
}

} // namespace pfs
