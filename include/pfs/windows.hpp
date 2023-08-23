////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2021.06.24 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once

#if _MSC_VER
#   define WIN32_LEAN_AND_MEAN
#   include <string>
#   include <windows.h>
#   include <windef.h>

namespace pfs {
namespace windows {

// https://stackoverflow.com/questions/215963/how-do-you-properly-use-widechartomultibyte
inline std::string utf8_encode (wchar_t const * s, int nwchars)
{
    if (!s)
        return std::string{};

    if (!nwchars)
        return std::string{};

    int nbytes = WideCharToMultiByte(CP_UTF8, 0, s
        , nwchars, nullptr, 0, nullptr, nullptr);

    if (!nbytes)
        return std::string{};

    std::string result(nbytes, '\0');

    nbytes = WideCharToMultiByte(CP_UTF8, 0, s
        , nwchars, & result[0], nbytes, nullptr, nullptr);

    if (!nbytes)
        return std::string{};

    return result;
}

inline std::string utf8_encode (wchar_t const * s)
{
    return utf8_encode(s, static_cast<int>(std::wcslen(s)));
}

// Convert an UTF8 string to a wide Unicode String
inline std::wstring utf8_decode (char const * s, int nchars /*const std::string & str*/)
{
    if (!s)
        return std::wstring{};

    if (!nchars)
        return std::wstring{};

    int nwchars = MultiByteToWideChar(CP_UTF8, 0, s, nchars, nullptr, 0);

    if (!nwchars)
        return std::wstring{};

    std::wstring result(nwchars, 0);

    nwchars = MultiByteToWideChar(CP_UTF8, 0, s, nchars, & result[0], nwchars);

    if (!nwchars)
        return std::wstring{};

    return result;
}

inline std::wstring utf8_decode (char const * s)
{
    auto n = std::strlen(s);
    return (s && n > 0) 
        ? utf8_decode(s, static_cast<int>(n))
        : std::wstring{};
}

inline std::string utf8_error (DWORD error_id)
{
    DWORD dwFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER
        | FORMAT_MESSAGE_FROM_SYSTEM
        | FORMAT_MESSAGE_IGNORE_INSERTS;

    wchar_t * pbuffer = nullptr;
    std::string result;

    do {
        auto nwchars = FormatMessageW(dwFlags, nullptr, error_id, 0
            , (wchar_t *)& pbuffer, 0, nullptr);

        if (nwchars == 0) {
            result = std::string{"Internal error: FormatMessageW() failed"};
            break;
        }

        result = utf8_encode(pbuffer, (int)nwchars);

        // Remove trailing '\r\n' symbols
        while (result[result.size()-1] == '\n' || result[result.size() - 1] == '\r')
            result.resize(result.size() - 1);
    } while(false);

    if (pbuffer)
        LocalFree(pbuffer);

    return result;
}

}} // namespace pfs::windows

#endif // _MSC_VER
