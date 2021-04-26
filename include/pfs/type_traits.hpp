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

// Attempt to implement is_function_pointer
// See https://stackoverflow.com/questions/6560590/is-function-pointer-for-type-traits
// This trait used to avoid overload ambiguity for emitter::connect() and solved the problem,
// but use it in another cases does not guarantee the expected result.
template <typename F>
struct is_function_pointer
{
    static constexpr bool value = std::is_pointer<F>::value
        ? std::is_function<typename std::remove_pointer<F>::type>::value
        : false;
};

template <typename F>
constexpr bool is_function_pointer<F>::value;

} // namespace pfs
