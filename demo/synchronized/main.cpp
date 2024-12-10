////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2024 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2024.12.09 Initial version.
////////////////////////////////////////////////////////////////////////////////
#include "pfs/synchronized.hpp"
#include <iostream>
#include <vector>

int main ()
{
    pfs::synchronized<std::vector<char>> safe;
    std::cout << "1. STEP" << std::endl;
    auto x1 = safe.rlock();
    std::cout << "2. STEP (BEFORE DEAD LOCK)" << std::endl;
    std::cout << "Press Ctrl+C to break the execution" << std::endl;
    auto x2 = safe.rlock();
    std::cout << "3. STEP (UNREACHED, DEAD LOCK)" << std::endl;

    return 0;
}
