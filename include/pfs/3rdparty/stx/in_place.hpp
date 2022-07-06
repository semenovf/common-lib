////////////////////////////////////////////////////////////////////////////////
// -*- C++ -*-
// Copyright (c) 2015, Just Software Solutions Ltd
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or
// without modification, are permitted provided that the
// following conditions are met:
//
// 1. Redistributions of source code must retain the above
// copyright notice, this list of conditions and the following
// disclaimer.
//
// 2. Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following
// disclaimer in the documentation and/or other materials
// provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of
// its contributors may be used to endorse or promote products
// derived from this software without specific prior written
// permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
// CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011 - 2012 Andrzej Krzemienski.
//
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// The idea and interface is based on Boost.Optional library
// authored by Fernando Luis Cacciola Carballal
////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2022 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2022.06.07 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once

namespace STX_NAMESPACE_NAME {

// 20.5.6, In-place construction
#ifndef STX_HAVE_IN_PLACE_T

struct in_place_t {
    explicit in_place_t() = default;
};

constexpr in_place_t in_place{};

template <class T> struct in_place_type_t {
    explicit in_place_type_t() = default;
};

template <size_t I> struct in_place_index_t {
    explicit in_place_index_t() = default;
};

#   if defined(__cpp_variable_templates) && __cpp_variable_templates >= 201304
template <typename T>
constexpr in_place_type_t<T> in_place_type{};

template <size_t I>
constexpr in_place_index_t<I> in_place_index{};

#       define STX_IN_PLACE_INDEX(I) in_place_index<I>

#   else

template <typename T>
inline constexpr in_place_type_t<T> in_place_type ()
{
    return in_place_type_t<T>{};
}

template <size_t I>
inline constexpr in_place_index_t<I> in_place_index ()
{
    return in_place_index_t<I>{};
}

#       define STX_IN_PLACE_INDEX(I) in_place_index<I>()
#   endif

#   define STX_HAVE_IN_PLACE_T
#else
// `std` implementation available
#   define STX_IN_PLACE_INDEX(I) std::in_place_index<I>
#endif

} // namespace STX_NAMESPACE_NAME
