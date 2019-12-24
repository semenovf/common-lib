////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 Vladislav Trifochkin
//
// This file is part of [pfs-common](https://github.com/semenovf/pfs-common) library.
//
// Changelog:
//      2019.12.17 Initial version
////////////////////////////////////////////////////////////////////////////////
#pragma once

#if __cplusplus < 201703L
#   include "3rdparty/variant.hpp"
#else
#   include <variant>
#endif

namespace pfs {

#if __cplusplus < 201703L
    namespace impl_variant_ns = mpark;
#else
    namespace impl_variant_ns = std;
#endif

constexpr std::size_t variant_npos = impl_variant_ns::variant_npos;

using in_place_t = impl_variant_ns::in_place_t;

template <std::size_t I>
using in_place_index_t = impl_variant_ns::in_place_index_t<I>;

template <typename T>
using in_place_type_t = impl_variant_ns::in_place_type_t<T>;

template <typename ...Types>
using variant = impl_variant_ns::variant<Types...>;

template <size_t I, typename T>
using variant_alternative_t = impl_variant_ns::variant_alternative_t<I, T>;

template <typename T, typename... Types>
inline constexpr bool holds_alternative (variant<Types...> const & v) noexcept
{
    return impl_variant_ns::holds_alternative<T, Types...>(v);
}

template <std::size_t I, typename... Types>
inline constexpr variant_alternative_t<I, variant<Types...>> & get (variant<Types...> & v)
{
    return impl_variant_ns::get<I, Types...>(v);
}

template <std::size_t I, typename... Types>
inline constexpr variant_alternative_t<I, variant<Types...>> && get (variant<Types...> && v)
{
    return impl_variant_ns::get<I, Types...>(std::forward<variant<Types...>>(v));
}

template <std::size_t I, typename... Types>
inline constexpr variant_alternative_t<I, variant<Types...>> const & get (variant<Types...> const & v)
{
    return impl_variant_ns::get<I, Types...>(v);
}

template <std::size_t I, typename... Types>
inline constexpr variant_alternative_t<I, variant<Types...>> const && get (variant<Types...> const && v)
{
    return impl_variant_ns::get<I, Types...>(std::forward<variant<Types...>>(v));
}

template <typename T, typename... Types>
constexpr T & get (variant<Types...>& v)
{
    return impl_variant_ns::get<T, Types...>(v);
}

template <typename T, typename... Types>
constexpr T && get (variant<Types...> && v)
{
    return impl_variant_ns::get<T, Types...>(std::forward<variant<Types...>>(v));
}

template <typename T, typename... Types>
constexpr T const & get (variant<Types...> const & v)
{
    return impl_variant_ns::get<T, Types...>(v);
}

template <typename T, typename... Types>
constexpr const T && get (variant<Types...> const && v)
{
    return impl_variant_ns::get<T, Types...>(std::forward<variant<Types...>>(v));
}

#if __cplusplus < 201703L

template <typename T>
using add_pointer_t = typename impl_variant_ns::lib::add_pointer_t<T>;

#else

template <typename T>
using add_pointer_t = typename impl_variant_ns::add_pointer_t<T>;

#endif

template <std::size_t I, typename... Types>
inline constexpr add_pointer_t<variant_alternative_t<I, variant<Types...>>>
    get_if (variant<Types...> * pv) noexcept
{
    return impl_variant_ns::get_if<I, Types...>(pv);
}

template <std::size_t I, typename... Types>
inline constexpr add_pointer_t<const variant_alternative_t<I, variant<Types...>>>
    get_if (variant<Types...> const * pv) noexcept
{
    return impl_variant_ns::get_if<I, Types...>(pv);
}

template <typename T, typename... Types>
inline constexpr add_pointer_t<T> get_if (variant<Types...> * pv) noexcept
{
    return impl_variant_ns::get_if<T, Types...>(pv);
}

template <typename T, typename... Types>
inline constexpr add_pointer_t<const T> get_if (variant<Types...> const * pv) noexcept
{
    return impl_variant_ns::get_if<T, Types...>(pv);
}

template <typename Visitor, typename ...Variants>
#ifdef MPARK_CPP14_CONSTEXPR
inline constexpr decltype(auto) visit (Visitor && visitor, Variants &&... vars)
#else
inline constexpr auto visit (Visitor && visitor, Variants &&... vars)
    -> decltype(impl_variant_ns::visit<Visitor, Variants...>(std::forward<Visitor>(visitor)
            , std::forward<Variants>(vars)...))
#endif
{
    return impl_variant_ns::visit<Visitor, Variants...>(std::forward<Visitor>(visitor)
            , std::forward<Variants>(vars)...);
}

} // namespace pfs
