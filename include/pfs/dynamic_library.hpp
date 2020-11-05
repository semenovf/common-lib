////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 Vladislav Trifochkin
//
// This file is part of [pfs-modulus](https://github.com/semenovf/pfs-modulus) library.
//
// Changelog:
//      2019.12.20 Initial version (inhereted from https://github.com/semenovf/pfs)
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "filesystem.hpp"
#include <list>
#include <string>
#include <system_error>

#if defined(_WIN32) || defined(_WIN64)
#   include <windows.h>
#   include <windef.h>
#   include <cassert>
#else
#   include <dlfcn.h>
#   include <sys/stat.h>
#   include <sys/types.h>
#   include <unistd.h>
#endif

// see http://en.wikipedia.org/wiki/Dynamic_loading
// [Dynamically Loaded C++ Objects](http://www.drdobbs.com/dynamically-loaded-c-objects/184401900?pgno=1)

namespace pfs {

#if defined(PFS_NO_STD_FILESYSTEM)
    namespace fs = pfs::filesystem;
#else
    namespace fs = std::filesystem;
#endif

////////////////////////////////////////////////////////////////////////////////
// dynamic_library_errc
////////////////////////////////////////////////////////////////////////////////

enum class dynamic_library_errc
{
      success = 0
    , invalid_argument
    , file_not_found
    , open_failed
    , symbol_not_found
};

#if defined(_WIN32) || defined(_WIN64)
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

inline std::string dlerror_win (DWORD error_id)
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

inline std::string dlerror ()
{
    return dlerror_win(GetLastError());
}
#else // POSIX
inline std::string dlerror ()
{
    return ::dlerror();
}
#endif

class dynamic_library_category : public std::error_category
{
public:
    virtual char const * name () const noexcept override
    {
        return "dynamic_library_category";
    }

    virtual std::string message (int ev) const override
    {
        switch (ev) {
            case static_cast<int>(dynamic_library_errc::success):
                return std::string{"no error"};

            case static_cast<int>(dynamic_library_errc::invalid_argument):
                return std::string{"invalid argument"};

            case static_cast<int>(dynamic_library_errc::file_not_found):
                return std::string{"shared object (dynamic library) not found"};

            case static_cast<int>(dynamic_library_errc::open_failed) :
                return std::string("failed to open shared object (dynamic library)");

            case static_cast<int>(dynamic_library_errc::symbol_not_found):
                return std::string("symbol not found in shared object (dynamic library)");

            default: return std::string{"unknown dynamic_library error"};
        }
    }

    static std::error_category const & category ()
    {
        static dynamic_library_category instance;
        return instance;
    }
};

inline std::error_code make_error_code (dynamic_library_errc e)
{
    return std::error_code(static_cast<int>(e)
        , dynamic_library_category::category());
}

////////////////////////////////////////////////////////////////////////////////
// dynamic_library
////////////////////////////////////////////////////////////////////////////////
class dynamic_library
{
public:
#if defined(_WIN32) || defined(_WIN64)
    using native_handle_type = HMODULE;
    using symbol_type = FARPROC;
#else
    using native_handle_type = void *;
    using symbol_type = void *;
#endif

private:
    native_handle_type _handle;
    fs::path _path;   // contains path to dynamic library
    std::string _native_error;

public:
    dynamic_library ()
        : _handle(0)
    {}

    dynamic_library (dynamic_library const &) = delete;
    dynamic_library & operator = (dynamic_library const &) = delete;

    ~dynamic_library ()
    {
        close();
    }

    native_handle_type native_handle () const
    {
        return _handle;
    }

    std::string const & native_error () const
    {
        return _native_error;
    }

    bool open (fs::path const & p, std::error_code & ec)
    {
        native_handle_type h{0};
        _native_error.clear();

        if (p.empty()) {
            ec = make_error_code(dynamic_library_errc::invalid_argument);
            return false;
        }

        if (!fs::exists(p)) {
            ec = pfs::make_error_code(dynamic_library_errc::file_not_found);
            return false;
        }

#if defined(_WIN32) || defined(_WIN64)

        DWORD dwFlags = 0;

        h = LoadLibraryEx(p.c_str(), NULL, dwFlags);

        if (!h) {
            ec = pfs::make_error_code(dynamic_library_errc::open_failed);
            _native_error = dlerror();
            return false;
        }

        _handle = h;
        _path = p;

#else // POSIX
        // clear error
        ::dlerror();

        bool global = false;
        bool resolve = true;

        h = ::dlopen(p.c_str(), (global ? RTLD_GLOBAL : RTLD_LOCAL)
                | ( resolve ? RTLD_NOW : RTLD_LAZY));

        if (!h) {
            ec = pfs::make_error_code(dynamic_library_errc::open_failed);
            _native_error = dlerror();
            return false;
        }

        _handle = h;
        _path = p;

#endif
        return true;
    }

    void close ()
    {
        _native_error.clear();

#if defined(_WIN32) || defined(_WIN64)
        if (_handle != native_handle_type{0}) {
            FreeLibrary(_handle);
            _handle = native_handle_type{0};
        }
#else // expected POSIX
        if (_handle != native_handle_type{ 0 }) {
            ::dlerror(); /*clear error*/
            ::dlclose(_handle);
            _handle = native_handle_type{ 0 };
        }
#endif
    }

    symbol_type resolve (char const * symbol_name, std::error_code & ec) noexcept
    {
        _native_error.clear();

#if defined(_WIN32) || defined(_WIN64)
        dynamic_library::symbol_type sym = GetProcAddress(_handle, symbol_name);

        if (!sym) {
            _native_error = dlerror();
            ec = pfs::make_error_code(dynamic_library_errc::symbol_not_found);
        }

        return sym;
#else // POSIX
        // clear error
        ::dlerror();

        dynamic_library::symbol_type sym = ::dlsym(_handle, symbol_name);

        // Failed to resolve symbol
        if (! sym) {
            _native_error = dlerror();
            ec = pfs::make_error_code(dynamic_library_errc::symbol_not_found);
        }

        return sym;
#endif
    }

    symbol_type resolve (char const * symbol_name)
    {
        std::error_code ec;
        symbol_type sym = resolve(symbol_name, ec);

        if (! sym) {
            throw std::system_error(ec
                , std::string("dynamic_library::resolve(): ")
#if defined(_WIN32) || defined(_WIN64)
                //+ _path.c_str() + ": " // FIXME
#else
                    + "path: " + _path.c_str() + "; "
#endif
                    + "symbol: " + symbol_name + "; "
                    + "error: " + _native_error);
        }

        return sym;
    }

    fs::path const & path () const noexcept
    {
        return _path;
    }

   /**
    * @brief Builds dynamic library (shared object) file name according to platform.
    *
    * @param name Base name of dynamic library.
    */
    static fs::path::string_type build_dl_filename (fs::path::string_type const & name) noexcept
    {
        fs::path::string_type result;

#if (defined(_WIN32) || defined(_WIN64)) && defined(_UNICODE)
        result += name;
        result += L".dll";
#else
        result += "lib";
        result += name;
        result += ".so";
#endif
        return result;
    }
};

//
// Need to avoid gcc compiler error:
// `error: ISO C++ forbids casting between pointer-to-function and pointer-to-object`
//
template <typename FuncPtr>
inline FuncPtr void_func_ptr_cast (void * p)
{
    static_assert(sizeof(void *) == sizeof(void (*)(void)),
                  "object pointer and function pointer sizes must be equal");
    union { void * p; FuncPtr f; } u;
    u.p = p;
    return u.f;
}

template <typename FuncPtr>
inline void * func_void_ptr_cast (FuncPtr f)
{
    static_assert(sizeof(void *) == sizeof(void (*)(void)),
                  "object pointer and function pointer sizes must be equal");
    union { void * p; FuncPtr f; } u;
    u.f = f;
    return u.p;
}

} // pfs
