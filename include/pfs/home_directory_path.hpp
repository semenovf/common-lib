////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2021.11.23 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "bits/operationsystem.h"
#include "filesystem.hpp"
#include "getenv.hpp"
#include <cstdlib>

#if PFS__COMPILER_GCC || PFS__COMPILER_CLANG
#   include <sys/types.h>
#   include <pwd.h>
#endif

namespace pfs {
namespace filesystem {

inline path home_directory_path ()
{
#if PFS__OS_WIN
        auto userprofile = wgetenv(L"USERPROFILE");

        if (userprofile.has_value()) {
            auto path = filesystem::path{*userprofile};

            if (filesystem::exists(path))
                return path;
        }

        auto homedrive = wgetenv(L"HOMEDRIVE");
        auto homepath  = wgetenv(L"HOMEPATH");

        if (homedrive.has_value() && homepath.has_value()) {
            auto path = filesystem::path{*homedrive} / *homepath;

            if (filesystem::exists(path))
                return path;
        }

        auto home = wgetenv(L"HOME");

        if (home.has_value()) {
            auto path = filesystem::path{*home};

            if (filesystem::exists(path))
                return path;
        }

        // Root path
        auto systemdrive = wgetenv(L"SystemDrive");
        filesystem::path path;

        if (systemdrive.has_value())
            path = *systemdrive + L'/';
        else
            path = L"c:/";

        return path;

#elif PFS__OS_LINUX
    auto home = getenv("HOME");

    if (home.has_value()) {
        auto path = filesystem::path{*home};

        if (filesystem::exists(path))
            return path;
    }

    struct passwd * pwd = getpwuid(getuid());

    if (pwd && pwd->pw_dir) {
        auto path = filesystem::path{pwd->pw_dir};
        return path;
    }
#endif // PFS__OS_WIN

    return filesystem::path{};
}

}} // namespace pfs::filesystem
