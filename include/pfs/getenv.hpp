////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021-2023 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2021.11.23 Initial version.
//      2023.08.23 Changed primary `getenv()`: pass C-style string instead of
//                 `std::string`. `getenv(std::string const &)` is overloaded now.
//                 Same for `wgetenv()`.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "bits/compiler.h"
#include "optional.hpp"
#include <array>
#include <string>
#include <cctype>
#include <cstdlib>
#include <cassert>

PFS__NAMESPACE_BEGIN

inline optional<std::string> getenv (char const * name)
{
#if PFS__COMPILER_MSVC

    std::size_t required_size = 0;

    ::getenv_s(& required_size, nullptr, 0, name);

    // Environment variable not set
    if (required_size == 0)
        return nullopt;

    std::string result;

    if (required_size <= 256) {
        std::array<char, 256> buffer;
        ::getenv_s(& required_size, buffer.data(), buffer.size(), name);

        // Remove trailing whitespaces
        while (std::isspace(buffer.data()[required_size - 1]))
            required_size--;

        result = std::string(buffer.data(), required_size);
    } else {
        char * buffer = new char[required_size];
        ::getenv_s(& required_size, buffer, required_size, name);

        // Remove trailing whitespaces
        while (std::isspace(buffer[required_size - 1]))
            required_size--;

        result = std::string(buffer, required_size);
        delete [] buffer;
    }

    return make_optional(std::move(result));

#elif defined(PFS__COMPILER_GCC)
#   if _GNU_SOURCE
    char * result = ::secure_getenv(name);
#   else
    char * result = ::getenv(name);
#   endif

    return result
        ? make_optional(std::string(result))
        : nullopt;
#elif defined(PFS__COMPILER_CLANG)
    char * result = ::getenv(name);

    return result
        ? make_optional(std::string(result))
        : nullopt;
#else
#   error "Unsupported compiler yet"
#endif // PFS__COMPILER_MSVC
}

inline optional<std::string> getenv (std::string const & name)
{
    return getenv(name.c_str());
}

#if PFS__COMPILER_MSVC

inline optional<std::wstring> wgetenv (wchar_t const * name)
{
    std::size_t required_size = 0;

    _wgetenv_s(& required_size, nullptr, 0, name);

    // Environment variable not set
    if (required_size == 0)
        return optional<std::wstring>{};

    std::wstring result;

    if (required_size <= 256) {
        std::array<wchar_t, 256> buffer;
        _wgetenv_s(& required_size, buffer.data(), buffer.size(), name);

        // Remove trailing whitespaces
        while (iswspace(buffer.data()[required_size - 1]))
            required_size--;

        result = std::wstring(buffer.data(), required_size);
    } else {
        wchar_t * buffer = new wchar_t[required_size];
        _wgetenv_s(& required_size, buffer, required_size, name);

        // Remove trailing whitespaces
        while (iswspace(buffer[required_size - 1]))
            required_size--;

        result = std::wstring(buffer, required_size);
        delete [] buffer;
    }

    return optional<std::wstring>(std::move(result));
}

inline optional<std::wstring> wgetenv (std::wstring const & name)
{
    return wgetenv(name.c_str());
}

#endif

PFS__NAMESPACE_END
