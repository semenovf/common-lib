////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2025.07.07 Initial version
////////////////////////////////////////////////////////////////////////////////
#include "pfs/fmt.hpp"
#include "pfs/hexdump.hpp"
#include <cstdlib>

int main (int, char * [])
{
    std::string s =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz\t0123456789[]{}=-+()~!@#$%^&*;.,<>/\\\n"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz\t0123456789[]{}=-+()~!@#$%^&*;.,<>/\\\n"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz\t0123456789[]{}=-+()~!@#$%^&*;.,<>/\\\n"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz\t0123456789[]{}=-+()~!@#$%^&*;.,<>/\\\n"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz\t0123456789[]{}=-+()~!@#$%^&*;.,<>/\\\n"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz\t0123456789[]{}=-+()~!@#$%^&*;.,<>/\\\n";

    auto dump = pfs::hexdump(s.begin(), s.end(), 16);
    fmt::println("{}", dump);

    dump = pfs::hexdump(s.begin(), s.end(), 32);
    fmt::println("{}", dump);

    return EXIT_SUCCESS;
}
