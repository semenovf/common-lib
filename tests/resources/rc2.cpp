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

extern "C" void rc2_init_resources ()
{
    pfs::resources::instance().add("b", b_data, b_length);
}
