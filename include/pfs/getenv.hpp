////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2021.11.23 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "bits/compiler.h"
#include "optional.hpp"
#include <array>
#include <string>
#include <cctype>
#include <cstdlib>
#include <cassert>

namespace pfs {

optional<std::string> getenv (std::string const & name)
{
#if PFS_COMPILER_MSVC

    std::size_t required_size = 0;

    ::getenv_s(& required_size, nullptr, 0, name.c_str());

    // Environment variable not set
    if (required_size == 0)
        return optional<std::string>{};

    std::string result;

    if (required_size <= 256) {
        std::array<char, 256> buffer;
        ::getenv_s(& required_size, buffer.data(), buffer.size(), name.c_str());

        // Remove trailing whitespaces
        while (std::isspace(buffer.data()[required_size - 1]));
            required_size--;

        result = std::string(buffer.data(), required_size);
    } else {
        char * buffer = new char[required_size];
        ::getenv_s(& required_size, buffer, required_size, name.c_str());

        // Remove trailing whitespaces
        while (std::isspace(buffer[required_size - 1]));
            required_size--;

        result = std::string(buffer, required_size);
        delete [] buffer;
    }

    return optional<std::string>(std::move(result));

#elif defined(PFS_COMPILER_GCC) && !defined(PFS_COMPILER_CLANG)
#   if _GNU_SOURCE
    char * result = ::secure_getenv(name.c_str());
#   else
    char * result = ::getenv(name.c_str());
#   endif

    return result
        ? optional<std::string>{std::string(result)}
        : optional<std::string>{};
#elif defined(PFS_COMPILER_CLANG)
    char * result = ::getenv(name.c_str());

    return result
        ? optional<std::string>{std::string(result)}
        : optional<std::string>{};
#else
#   error "Unsupported platform yet"
#endif // PFS_COMPILER_GCC
}

#if PFS_COMPILER_MSVC

optional<std::wstring> wgetenv (std::wstring const & name)
{
    std::size_t required_size = 0;

    _wgetenv_s(& required_size, nullptr, 0, name.c_str());

    // Environment variable not set
    if (required_size == 0)
        return optional<std::wstring>{};

    std::wstring result;

    if (required_size <= 256) {
        std::array<wchar_t, 256> buffer;
        _wgetenv_s(& required_size, buffer.data(), buffer.size(), name.c_str());

        // Remove trailing whitespaces
        while (iswspace(buffer.data()[required_size - 1]));
            required_size--;

        result = std::wstring(buffer.data(), required_size);
    } else {
        wchar_t * buffer = new wchar_t[required_size];
        _wgetenv_s(& required_size, buffer, required_size, name.c_str());

        // Remove trailing whitespaces
        while (iswspace(buffer[required_size - 1]));
            required_size--;

        result = std::wstring(buffer, required_size);
        delete [] buffer;
    }

    return optional<std::wstring>(std::move(result));
}
#endif

} // namespace pfs

