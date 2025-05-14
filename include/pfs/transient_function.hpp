////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2025.05.06 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "namespace.hpp"
#include "3rdparty/functional/TransientFunction.hpp"

PFS__NAMESPACE_BEGIN

template <typename T>
using transient_function = TransientFunction<T>;

PFS__NAMESPACE_END
