////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2022 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2019.12.23 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "i128.hpp"

#if PFS_HAS_INT128
#   ifndef FMT_USE_INT128
#       define FMT_USE_INT128 1
#   endif
#endif

#define FMT_HEADER_ONLY 1
#include "3rdparty/fmt/format.h"
#include "3rdparty/fmt/chrono.h"
