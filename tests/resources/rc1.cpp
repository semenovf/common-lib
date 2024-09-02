////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2024 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2024.07.21 Initial version.
////////////////////////////////////////////////////////////////////////////////
#include "pfs/resources.hpp"

static std::uint8_t a_data[] = { 0x01, 0x02, 0x03 };
static std::size_t a_length = 3;

extern "C" void rc1_init_resources (pfs::resources & instance)
{
    instance.add("a", a_data, a_length);
}
