////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2023 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2023.07.16 Initial version.
////////////////////////////////////////////////////////////////////////////////
#include "pfs/traverse_directory.hpp"
#include "pfs/fmt.hpp"

namespace fs = pfs::filesystem;

int main (int argc, char * argv[])
{
    if (argc < 2) {
        fmt::print(stderr, "Too few arguments\nRun {} DIR\n", argv[0]);
        return EXIT_FAILURE;
    }

    auto initial_dir = fs::utf8_decode(argv[1]);

    fs::directory_traversal dt;

    dt.on_enter_directory = [] (fs::path const & dir) {
        fmt::print("Entering directory: {}\n", dir);
    };

    dt.on_leave_directory = [] (fs::path const & dir) {
        fmt::print("Leave directory: {}\n", dir);
    };

    dt.on_entry = [] (pfs::filesystem::path const & path) {
        fmt::print("ENTRY: {}\n", path);
    };

    dt.on_entry_error = [] (fs::path const & path, pfs::error const & err) {
        fmt::print(stderr, "ERROR: {}: {}\n", path, err.what());
    };

    dt.on_error = [] (pfs::error const & err) {
        fmt::print(stderr, "ERROR: {}\n", err.what());
    };

    dt.traverse(initial_dir);

    return EXIT_SUCCESS;
}
