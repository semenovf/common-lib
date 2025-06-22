////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020-2023 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2023.07.17 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "bits/operationsystem.h"
#include "error.hpp"
#include "filesystem.hpp"
#include "getenv.hpp"

namespace pfs {
namespace filesystem {

// https://en.wikipedia.org/wiki/Home_directory
// https://learn.microsoft.com/en-us/windows/win32/api/shlobj_core/nf-shlobj_core-shgetknownfolderpath?redirectedfrom=MSDN

class standard_paths final
{
public:
    /**
     * The home folder, or an empty path if it cannot be determined.
     */
    static path home_folder ()
    {
#if PFS__OS_LINUX
        auto home_env = getenv("HOME");

        if (!home_env)
            return path{};

        if (home_env->empty())
            return path{};

        return utf8_decode_path(*home_env);
#elif PFS__OS_WIN
        auto userprofile_env = getenv("USERPROFILE");

        if (userprofile_env && !userprofile_env->empty()) {
            auto home = utf8_decode_path(*userprofile_env);

            if (exists(home))
                return home;
        }

        auto homedrive_env = getenv("HOMEDRIVE");
        auto homepath_env  = getenv("HOMEPATH");

        if (homedrive_env && !homedrive_env->empty() && homepath_env && !homepath_env->empty()) {
            auto home = utf8_decode_path(*homedrive_env + *homepath_env);

            if (exists(home))
                return home;
        }

        auto home_env = getenv("HOME");

        if (home_env && !home_env->empty()) {
            auto home = utf8_decode_path(*home_env);

            if (exists(home))
                return home;
        }

        return path{};
#else
        return path{};
#endif
    }

    /**
     * The temporary folder, or an empty path if it cannot be determined.
     */
    static path temp_folder ()
    {
#if PFS__OS_LINUX
        return utf8_decode_path("/tmp");
#elif PFS__OS_WIN
        auto temp_env = getenv("TEMP");

        if (temp_env && !temp_env->empty()) {
            auto temp = utf8_decode_path(*temp_env);

            if (exists(temp))
                return temp;
        }

        return path{};
#else
        return path{};
#endif
    }

    /**
     * The download folder, or an empty path if it cannot be determined.
     */
    static path download_folder ()
    {
        auto home = home_folder();
#if PFS__OS_LINUX
        return home.empty() ? path{} : home / utf8_decode_path("Downloads");
#elif PFS__OS_WIN
        return home.empty() ? path{} : home / utf8_decode_path("Downloads");
#else
        return path{};
#endif
    }

    /**
     * The desktop folder, or an empty path if it cannot be determined.
     */
    static path desktop_folder ()
    {
        auto home = home_folder();
#if PFS__OS_LINUX
        return home.empty() ? path{} : home / utf8_decode_path("Desktop");
#elif PFS__OS_WIN
        return home.empty() ? path{} : home / utf8_decode_path("Desktop");
#else
        return path{};
#endif
    }

    /**
     * The documents folder, or an empty path if it cannot be determined.
     */
    static path documents_folder ()
    {
        auto home = home_folder();
#if PFS__OS_LINUX
        return home.empty() ? path{} : home / utf8_decode_path("Documents");
#elif PFS__OS_WIN
        return home.empty() ? path{} : home / utf8_decode_path("Documents");
#else
        return path{};
#endif
    }

    /**
     * The music folder, or an empty path if it cannot be determined.
     */
    static path music_folder ()
    {
        auto home = home_folder();
#if PFS__OS_LINUX
        return home.empty() ? path{} : home / utf8_decode_path("Music");
#elif PFS__OS_WIN
        return home.empty() ? path{} : home / utf8_decode_path("Music");
#else
        return path{};
#endif
    }

    /**
     * The movies folder, or an empty path if it cannot be determined.
     */
    static path movies_folder ()
    {
        auto home = home_folder();
#if PFS__OS_LINUX
        return home.empty() ? path{} : home / utf8_decode_path("Videos");
#elif PFS__OS_WIN
        return home.empty() ? path{} : home / utf8_decode_path("Videos");
#else
        return path{};
#endif
    }

    /**
     * The pictures folder, or an empty path if it cannot be determined.
     */
    static path pictures_folder ()
    {
        auto home = home_folder();
#if PFS__OS_LINUX
        return home.empty() ? path{} : home / utf8_decode_path("Pictures");
#elif PFS__OS_WIN
        return home.empty() ? path{} : home / utf8_decode_path("Pictures");
#else
        return path{};
#endif
    }

    /**
     * Root data folder, or an empty path if it cannot be determined.
     */
    static path data_folder ()
    {
        auto home = home_folder();
#if PFS__OS_LINUX
        return home.empty() ? path{} : home / utf8_decode_path(".local/share");
#elif PFS__OS_WIN
        return home.empty() ? path{} : home / utf8_decode_path("AppData\\Local");
#else
        return path{};
#endif
    }

    /**
     * Root cache folder, or an empty path if it cannot be determined.
     */
    static path cache_folder ()
    {
        auto home = home_folder();
#if PFS__OS_LINUX
        return home.empty() ? path{} : home / utf8_decode_path(".cache");
#elif PFS__OS_WIN
        return home.empty() ? path{} : home / utf8_decode_path("AppData\\Local");
#else
        return path{};
#endif
    }

    /**
     * Root config folder, or an empty path if it cannot be determined.
     */
    static path config_folder ()
    {
        auto home = home_folder();
#if PFS__OS_LINUX
        return home.empty() ? path{} : home / utf8_decode_path(".config");
#elif PFS__OS_WIN
        return home.empty() ? path{} : home / utf8_decode_path("AppData\\Local");
#else
        return path{};
#endif
    }
};

}} // namespace pfs::filesystem
