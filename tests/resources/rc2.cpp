////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2024 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2024.07.21 Initial version.
////////////////////////////////////////////////////////////////////////////////
#include "pfs/resources.hpp"

static std::uint8_t b_data[] = { 0x04, 0x05, 0x06, 0x07 };
static std::size_t b_length = 4;

#if _MSC_VER
#   define DLL_EXPORT __declspec(dllexport)
#else
#   define DLL_EXPORT
#endif

extern "C" DLL_EXPORT void rc2_init_resources (pfs::resources & instance)
{
    instance.add("b", b_data, b_length);
}
