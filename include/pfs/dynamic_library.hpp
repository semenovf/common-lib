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
#else
#   include <dlfcn.h>
#   include <sys/stat.h>
#   include <sys/types.h>
#   include <unistd.h>
#endif

// see http://en.wikipedia.org/wiki/Dynamic_loading
// [Dynamically Loaded C++ Objects](http://www.drdobbs.com/dynamically-loaded-c-objects/184401900?pgno=1)

namespace pfs {

namespace fs = std::filesystem;

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

            case static_cast<int>(dynamic_library_errc::open_failed):
                return std::string("failed to open shared object (dynamic library): ")
                        + dlerror();

            case static_cast<int>(dynamic_library_errc::symbol_not_found):
                return std::string("symbol not found in shared object (dynamic library): ")
                        + dlerror();

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
    using string_type = std::string;
    using stringlist_type = std::list<string_type>;

#if defined(_WIN32) || defined(_WIN64)
    //using string_type = std::wstring;
    using native_handle_type = HMODULE;
    using symbol_type = FARPROC;
#else
    using native_handle_type = void *;
    using symbol_type = void *;
#endif

public:
    dynamic_library ()
        : _handle(0)
    {}

    dynamic_library (dynamic_library const &) = delete;
    dynamic_library & operator = (dynamic_library const &) = delete;

    ~dynamic_library ()
    {
#if defined(_WIN32) || defined(_WIN64)
#   error "Need to implement for Windows"
#else
        // POSIX implementation
        if (_handle) {
            ::dlerror(); /*clear error*/
            ::dlclose(_handle);
            _handle = dynamic_library::native_handle_type(0);
        }
#endif
    }

    native_handle_type native_handle () const
    {
        return _handle;
    }

    bool open (fs::path const & p, std::error_code & ec)
    {
#if defined(_WIN32) || defined(_WIN64)
#   error "Need to implement for Windows"
#else // POSIX
        dynamic_library::native_handle_type h(0);

        if (p.empty()) {
            ec = make_error_code(dynamic_library_errc::invalid_argument);
            return false;
        }

        if (!fs::exists(p)) {
            ec = pfs::make_error_code(dynamic_library_errc::file_not_found);
            return false;
        }

        // clear error
        ::dlerror();

        bool global = false;
        bool resolve = true;

        h = ::dlopen(p.c_str(), (global ? RTLD_GLOBAL : RTLD_LOCAL)
                | ( resolve ? RTLD_NOW : RTLD_LAZY));

        if (!h) {
            ec = pfs::make_error_code(dynamic_library_errc::open_failed);
            return false;
        }

        _handle = h;
        _path = p;

#endif
        return true;
    }

    symbol_type resolve (char const * symbol_name, std::error_code & ec) noexcept
    {
#if defined(_WIN32) || defined(_WIN64)
#   error "Need to implement for Windows"
#else // POSIX
        // clear error
        ::dlerror();

        dynamic_library::symbol_type sym = ::dlsym(_handle, symbol_name);

        // Failed to resolve symbol
        if (! sym)
            ec = pfs::make_error_code(dynamic_library_errc::symbol_not_found);

        return sym;
#endif
    }

    symbol_type resolve (char const * symbol_name)
    {
        std::error_code ec;
        symbol_type sym = resolve(symbol_name, ec);

        if (! sym) {
#if defined(_WIN32) || defined(_WIN64)
#   error "Need to implement for Windows"
#else
            throw std::system_error(ec
                , std::string("dynamic_library::resolve(): ")
                    + _path.c_str() + ": "
                    + std::string(::dlerror()));
#endif
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
    static string_type build_dl_filename (dynamic_library::string_type const & name) noexcept
    {
        string_type result;
#if defined(_WIN32) || defined(_WIN64)
#else
        result += "lib";
        result += name;
        result += ".so";
#endif
        return result;
    }

private:
    native_handle_type _handle;
    fs::path _path;   // contains path to dynamic library
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
