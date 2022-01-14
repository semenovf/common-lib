////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2019.12.20 Initial version (inhereted from https://github.com/semenovf/pfs)
//      2021.12.29 Refactored according to RAII idiom.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "error.hpp"
#include "filesystem.hpp"
#include <list>
#include <string>

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

//
// Need to avoid gcc compiler error:
// `error: ISO C++ forbids casting between pointer-to-function and pointer-to-object`
//
template <typename FuncPtr>
inline FuncPtr function_pointer_cast (void * p)
{
    static_assert(sizeof(void *) == sizeof(void (*)(void))
        , "object pointer and function pointer sizes must be equal");
    union { void * p; FuncPtr f; } u;
    u.p = p;
    return u.f;
}

// template <typename FuncPtr>
// inline void * func_void_ptr_cast (FuncPtr f)
// {
//     static_assert(sizeof(void *) == sizeof(void (*)(void))
//         , "object pointer and function pointer sizes must be equal");
//     union { void * p; FuncPtr f; } u;
//     u.f = f;
//     return u.p;
// }

class dynamic_library final
{
public:
#if _MSC_VER
    using native_handle_type = HMODULE;
    using symbol_type = FARPROC;
#else
    using native_handle_type = void *;
    using symbol_type = void *;
#endif

private:
    native_handle_type _handle;

private:
#if _MSC_VER
    static std::string last_error ()
    {
        return windows::utf8_error(GetLastError());
    }
#else // POSIX
    static std::string last_error ()
    {
        return ::dlerror();
    }
#endif

private:
    symbol_type resolve_impl (char const * symbol_name, error * perr = nullptr)
    {
        auto success = true;

#if _MSC_VER
        dynamic_library::symbol_type sym = GetProcAddress(_handle, symbol_name);

        if (!sym)
            success = false;
#else // POSIX
        // clear error
        ::dlerror();

        dynamic_library::symbol_type sym = ::dlsym(_handle, symbol_name);

        // Failed to resolve symbol
        if (! sym)
            success = false;
#endif

        if (!sym) {
            auto ec = pfs::make_error_code(dynamic_library_errc::symbol_not_found);
            auto err = error{ec, last_error()};
            if (perr) *perr = err; else PFS__THROW(err);
        }

        return sym;
    }

public:
    dynamic_library (fs::path const & p, error * perr = nullptr)
        : _handle(0)
    {
        native_handle_type h{0};

        if (p.empty()) {
            auto ec = make_error_code(dynamic_library_errc::invalid_argument);
            auto err = error{ec};
            if (perr) *perr = err; else PFS__THROW(err);
            return;
        }

        if (!fs::exists(p)) {
            auto ec = pfs::make_error_code(dynamic_library_errc::file_not_found);
            auto err = error{ec};
            if (perr) *perr = err; else PFS__THROW(err);
            return;
        }

#if _MSC_VER
        DWORD dwFlags = 0;

        h = LoadLibraryEx(p.c_str(), NULL, dwFlags);

        if (!h) {
            auto ec = pfs::make_error_code(dynamic_library_errc::open_failed);
            auto err = error{ec, last_error()};
            if (perr) *perr = err; else PFS__THROW(err);
            return;
        }

        _handle = h;

#else // POSIX
        // clear error
        ::dlerror();

        bool global = false;
        bool resolve = true;

        h = ::dlopen(p.c_str(), (global ? RTLD_GLOBAL : RTLD_LOCAL)
            | ( resolve ? RTLD_NOW : RTLD_LAZY));

        if (!h) {
            auto ec = pfs::make_error_code(dynamic_library_errc::open_failed);
            auto err = error{ec, last_error()};
            if (perr) *perr = err; else PFS__THROW(err);
            return;
        }

        _handle = h;
#endif
    }

    dynamic_library () = delete;
    dynamic_library (dynamic_library const &) = delete;
    dynamic_library & operator = (dynamic_library const &) = delete;

    dynamic_library (dynamic_library && other)
        : _handle(other._handle)
    {
        other._handle = native_handle_type{0};
    }

    dynamic_library & operator = (dynamic_library && other)
    {
        _handle = other._handle;
        other._handle = native_handle_type{0};
        return *this;
    }

    ~dynamic_library ()
    {
#if _MSC_VER
        if (_handle != native_handle_type{0}) {
            FreeLibrary(_handle);
            _handle = native_handle_type{0};
        }
#else // expected POSIX
        if (_handle != native_handle_type{0}) {
            ::dlerror(); /*clear error*/
            ::dlclose(_handle);
            _handle = native_handle_type{0};
        }
#endif
    }

    operator bool () const noexcept
    {
        return _handle != native_handle_type{0};
    }

    template <typename Signature>
    Signature * resolve (char const * symbol_name, error * perr = nullptr)
    {
        return function_pointer_cast<Signature *>(resolve_impl(symbol_name, perr));
    }

    template <typename Signature>
    Signature * resolve (std::string const & symbol_name, error * perr = nullptr)
    {
        return function_pointer_cast<Signature *>(resolve_impl(symbol_name.c_str(), perr));
    }

public:
   /**
    * @brief Builds dynamic library (shared object) file name according to platform.
    *
    * @param name Base name of dynamic library.
    */
    static fs::path build_filename (std::string const & name) noexcept
    {
        fs::path::string_type result;

#if _MSC_VER
#   if defined(_UNICODE)
        result += windows::utf8_decode(name.c_str(), static_cast<int>(name.size()));
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
        return fs::path{result};
    }
};

} // pfs
