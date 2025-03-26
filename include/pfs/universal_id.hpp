////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021-2024 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2021.08.14 Initial version (in `chat-lib`).
//      2021.10.01 Moved from `chat-lib`.
//      2021.10.03 Added calculation CRC32 and CRC64.
//      2021.11.08 Added `make_uuid` - UUID construction functions.
//      2024.12.23 Content moved into `universal_id_ulid.hpp`.
//                 Now this is a meta-header for different implementations.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#if defined(PFS__UNIVERSAL_ID_IMPL_UUIDV7)
#   include "universal_id_uuidv7.hpp"
#else
#   include "universal_id_ulid.hpp"
#endif
