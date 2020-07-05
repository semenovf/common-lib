////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of [pfs-common](https://github.com/semenovf/pfs-common) library.
//
// Changelog:
//      2020.07.05 Initial version
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <type_traits>

namespace pfs {

#if __cplusplus < 201402L
template <typename T>
using remove_cv_t = typename std::remove_cv<T>::type;
#endif

#if __cplusplus < 201402L
template <typename T>
using remove_reference_t = typename std::remove_reference<T>::type;
#endif

#if __cplusplus <= 201703L
template <typename T>
struct remove_cvref
{
    using type = remove_cv_t<remove_reference_t<T>>;
};

template <typename T>
using remove_cvref_t = typename remove_cvref<T>::type;
#endif

} // namespace pfs
