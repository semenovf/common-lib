////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020,2021 Vladislav Trifochkin
//
// This file is part of [common-lib](https://github.com/semenovf/common-lib) library.
//
// Changelog:
//      2020.07.05 Initial version
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <type_traits>

namespace pfs {

template <typename T>
using remove_cv_t = typename std::remove_cv<T>::type;

template <typename T>
using remove_reference_t = typename std::remove_reference<T>::type;

template <typename T>
struct remove_cvref
{
    using type = remove_cv_t<remove_reference_t<T>>;
};

template <typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

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
