////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020-2021 Vladislav Trifochkin
//
// This file is part of [common-lib](https://github.com/semenovf/common-lib) library.
//
// Changelog:
//      2021.05.25 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <memory>
#include <type_traits>

namespace pfs {

#if __cplusplus > 201103L
    using ::std::make_unique;
#else
    // see [std::make_unique](http://en.cppreference.com/w/cpp/memory/unique_ptr/make_unique)
    // `Possible Implementation` section.
//     template<typename T, typename ...Args>
//     inline std::unique_ptr<T> make_unique (Args &&... args)
//     {
//         return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
//     }

namespace details {

// gcc implementation
template <typename T>
struct make_unique_traits
{
    using single_type = std::unique_ptr<T>;
};

template <typename T>
struct make_unique_traits<T[]>
{
    using array_type = std::unique_ptr<T[]>;
};

template <typename T, size_t N>
struct make_unique_traits<T[N]>
{
    struct invalid_type {};
};

} // namespace details

template <typename T, typename ...Args>
inline typename details::make_unique_traits<T>::single_type
make_unique (Args &&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template <typename T>
inline typename details::make_unique_traits<T>::array_type
make_unique (size_t n)
{
    return std::unique_ptr<T>(new typename std::remove_extent<T>::type[n]());
}

/// Disable std::make_unique for arrays of known bound
template <typename T, typename ...Args>
inline typename details::make_unique_traits<T>::invalid_type
make_unique (Args &&...) = delete;

#endif

} // namespace pfs
