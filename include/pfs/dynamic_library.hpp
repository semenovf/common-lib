////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 Vladislav Trifochkin
//
// This file is part of [common-lib](https://github.com/semenovf/common-lib) library.
//
// Changelog:
//      2019.12.20 Initial version (inhereted from https://github.com/semenovf/pfs)
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "filesystem.hpp"
#include <list>
#include <string>
#include <system_error>

#if _MSC_VER
#   include "windows.hpp"
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
    namespace fs = filesystem;

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

#if _MSC_VER
inline std::string dlerror ()
{
    return windows::utf8_error(GetLastError());
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

#if _MSC_VER

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

#if _MSC_VER
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

#if _MSC_VER
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
#if _MSC_VER
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
    static fs::path::string_type build_filename (fs::path::string_type const & name) noexcept
    {
        fs::path::string_type result;

#if _MSC_VER
#   if defined(_UNICODE)
        result += name;
        result += L".dll";
#   else
        result += name;
        result += ".dll";
#   endif
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
