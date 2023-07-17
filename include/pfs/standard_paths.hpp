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
    static path home_folder ()
    {
#if PFS__OS_LINUX
        auto home_env = getenv("HOME");

        if (!home_env)
            return utf8_decode("/");

        if (home_env->empty())
            return utf8_decode("/");

        return utf8_decode(*home_env);
#elif PFS__OS_WIN
        auto userprofile_env = getenv("USERPROFILE");

        if (userprofile_env && !userprofile_env->empty()) {
            auto home = utf8_decode(userprofile_env);

            if (fs::exists(home))
                return home;
        }

        auto homedrive_env = getenv("HOMEDRIVE");
        auto homepath_env  = getenv("HOMEPATH");

        if (homedrive_env && !homedrive_env->empty() && homepath_env && !homepath_env->empty()) {
            auto home = utf8_decode(*homedrive_env + *homepath_env);

            if (fs::exists(home))
                return home;
        }

        auto home_env = getenv("HOME");

        if (home_env && !home_env->empty()) {
            auto home = utf8_decode(home_env);

            if (fs::exists(home))
                return home;
        }

        return utf8_decode("C:\\");

#else
#   error "Unsupported operation system"
#endif
    }

    /**
     * Folder where temporary files can be stored.
     */
    static path temp_folder ()
    {
#if PFS__OS_LINUX
        return utf8_decode("/tmp");
#elif PFS__OS_WIN
        auto temp_env = getenv("TEMP");

        if (temp_env && !temp_env->empty()) {
            auto temp = utf8_decode(temp_env);

            if (fs::exists(temp))
                return temp;
        }

        return utf8_decode("C:\\");
#else
#   error "Unsupported operation system"
#endif
    }

    static path download_folder ()
    {
#if PFS__OS_LINUX
        return home_folder() / utf8_decode("Downloads");
#elif PFS__OS_WIN
        return home_folder() / utf8_decode("Downloads");
#else
#   error "Unsupported operation system"
#endif
    }

    static path desktop_folder ()
    {
#if PFS__OS_LINUX
        return home_folder() / utf8_decode("Desktop");
#elif PFS__OS_WIN
        return home_folder() / utf8_decode("Desktop");
#else
#   error "Unsupported operation system"
#endif
    }

    static path documents_folder ()
    {
#if PFS__OS_LINUX
        return home_folder() / utf8_decode("Documents");
#elif PFS__OS_WIN
        return home_folder() / utf8_decode("Documents");
#else
#   error "Unsupported operation system"
#endif
    }

    static path music_folder ()
    {
#if PFS__OS_LINUX
        return home_folder() / utf8_decode("Music");
#elif PFS__OS_WIN
        return home_folder() / utf8_decode("Music");
#else
#   error "Unsupported operation system"
#endif
    }

    static path movies_folder ()
    {
#if PFS__OS_LINUX
        return home_folder() / utf8_decode("Videos");
#elif PFS__OS_WIN
        return home_folder() / utf8_decode("Videos");
#else
#   error "Unsupported operation system"
#endif
    }

    static path pictures_folder ()
    {
#if PFS__OS_LINUX
        return home_folder() / utf8_decode("Pictures");
#elif PFS__OS_WIN
        return home_folder() / utf8_decode("Pictures");
#else
#   error "Unsupported operation system"
#endif
    }

    /**
     * Application data folder
     */
    static path data_folder ()
    {
#if PFS__OS_LINUX
        return home_folder() / utf8_decode(".local/share");
#elif PFS__OS_WIN
        return home_folder() / utf8_decode("AppData\\Local");
#else
#   error "Unsupported operation system"
#endif
    }

    static path cache_folder ()
    {
#if PFS__OS_LINUX
        return home_folder() / utf8_decode(".cache");
#elif PFS__OS_WIN
        return home_folder() / utf8_decode("AppData\\Local");
#else
#   error "Unsupported operation system"
#endif
    }

    static path config_folder ()
    {
#if PFS__OS_LINUX
        return home_folder() / utf8_decode(".config");
#elif PFS__OS_WIN
        return home_folder() / utf8_decode("AppData\\Local");
#else
#   error "Unsupported operation system"
#endif
    }
};

}} // namespace pfs::filesystem

