////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2022 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2022.08.07 Initial version
////////////////////////////////////////////////////////////////////////////////
#include "pfs/filesystem.hpp"
#include "pfs/sha256.hpp"
#include "pfs/stopwatch.hpp"
#include <iostream>

namespace fs = pfs::filesystem;

int main (int argc, char * argv[])
{
    if (argc < 2) {
        std::cerr << "Too few arguments"
            << "\nRun `" << argv[0] << " <file>`\n";
        return EXIT_FAILURE;
    }

    auto path = pfs::utf8_decode_path(argv[1]);

    if (!fs::exists(path)) {
        std::cerr << "File not found: " << pfs::utf8_encode_path(path) << "\n";
        return EXIT_FAILURE;
    }

    pfs::stopwatch<> sw;

    sw.start();

    auto digest = pfs::crypto::sha256::digest(path);

    sw.stop();

    std::cout << to_string(digest) << " in " << sw.count() << " microseconds ("
        << static_cast<double>(sw.count()) / 1000.0 / 1000.0 << " seconds)\n";

    return EXIT_SUCCESS;
}
