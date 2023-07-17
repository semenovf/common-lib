////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2023 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2023.07.17 Initial version.
////////////////////////////////////////////////////////////////////////////////
#include "pfs/standard_paths.hpp"
#include "pfs/fmt.hpp"

namespace fs = pfs::filesystem;

int main (int argc, char * argv[])
{
    fmt::print("Home folder     : {}\n", fs::standard_paths::home_folder());
    fmt::print("Temp folder     : {}\n", fs::standard_paths::temp_folder());
    fmt::print("Download folder : {}\n", fs::standard_paths::download_folder());
    fmt::print("Desktop folder  : {}\n", fs::standard_paths::desktop_folder());
    fmt::print("Documents folder: {}\n", fs::standard_paths::documents_folder());
    fmt::print("Music folder    : {}\n", fs::standard_paths::music_folder());
    fmt::print("Movies folder   : {}\n", fs::standard_paths::movies_folder());
    fmt::print("Pictures folder : {}\n", fs::standard_paths::pictures_folder());
    fmt::print("Data folder     : {}\n", fs::standard_paths::data_folder());
    fmt::print("Cache folder    : {}\n", fs::standard_paths::cache_folder());
    fmt::print("Config folder   : {}\n", fs::standard_paths::config_folder());

    return EXIT_SUCCESS;
}
