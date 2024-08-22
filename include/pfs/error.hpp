////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2021.12.29 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "assert.hpp"
#include <string>
#include <system_error>
#include <string.h>

#if _MSC_VER
#   include "windows.hpp"
#endif

namespace pfs {

using error_code = std::error_code;

enum class errc
{
      success = 0
    , system_error      // More information can be obtained using errno (Linux) or
                        // WSAGetLastError (Windows)
    , broken_sequence
    , jni_error         // JNI specific error
    , unexpected_data
    , unexpected_error  // Replaces any unexpected error
};

class error_category : public std::error_category
{
public:
    virtual char const * name () const noexcept override
    {
        return "pfs::common::category";
    }

    virtual std::string message (int ev) const override
    {
        switch (static_cast<errc>(ev)) {
            case errc::success:
                return std::string{"no error"};

            case errc::system_error:
                return std::string{"system error"};
              
            case errc::broken_sequence:
                return std::string{"broken sequence"};

            case errc::jni_error:
                return std::string{"JNI error"};

            case errc::unexpected_data:
                return std::string{"unexpected data"};

            case errc::unexpected_error:
                return std::string{"unexpected error"};

            default: return std::string{"unknown common error"};
        }
    }
};

inline std::error_category const & get_error_category ()
{
    static error_category instance;
    return instance;
}

inline std::error_code make_error_code (errc e)
{
    return std::error_code(static_cast<int>(e), get_error_category());
}

/**
 * @note This exception class may throw std::bad_alloc as it uses std::string.
 */
class error: public std::system_error
{
public:
    error () : std::system_error(make_error_code(errc::success))
    {}

    error (std::error_code ec)
        : std::system_error(ec)
    {}

    error (errc e)
        : std::system_error(make_error_code(e))
    {}

    error (std::error_code ec
        , std::string const & description
        , std::string const & cause)
        : std::system_error(ec, description + " (" + cause  + ')')
    {}

    error (errc e
        , std::string const & description
        , std::string const & cause)
        : std::system_error(make_error_code(e), description + " (" + cause  + ')')
    {}

    error (std::error_code ec
        , std::string const & description)
        : std::system_error(ec, description)
    {}

    error (errc e
        , std::string const & description)
        : std::system_error(make_error_code(e), description)
    {}

    error (error const & other ) /*noexcept*/ = default;
    error (error && other ) /*noexcept*/ = default;
    error & operator = (error const & other ) /*noexcept*/ = default;
    error & operator = (error && other ) /*noexcept*/ = default;

    operator bool () const noexcept
    {
        return this->code().value() != 0;
    }
};

inline error make_exception (errc e)
{
    return error(make_error_code(e));
}

inline error_code get_last_system_error ()
{
#if _MSC_VER
    return error_code{static_cast<int>(::GetLastError()), std::system_category()};
#else // _MSC_VER
    return error_code{errno, std::generic_category()};
#endif // POSIX
}

#if _MSC_VER
#else
//
// See ghc::filesystem
//
inline char const * strerror_adapter (char * gnu, char *)
{
    return gnu;
}

inline char const * strerror_adapter (int posix, char * buffer)
{
    if (posix)
        return "Error in strerror_r!";
    return buffer;
}

#endif

inline std::string system_error_text (int errn = 0)
{
#if _MSC_VER
    return windows::utf8_error(errn == 0 ? GetLastError() : static_cast<DWORD>(errn));
#else // POSIX
    char buffer[256];
    strerror_r(errn == 0 ? errno : errn, buffer, sizeof(buffer));
    return strerror_adapter(strerror_r(errn == 0 ? errno : errn, buffer, sizeof(buffer)), buffer);
#endif
}

template <typename Error, typename ...Args>
inline void throw_or (Error * perr, Args &&... args)
{
    Error err (std::forward<Args>(args)...);

    if (perr) {
        *perr = std::move(err);
        return;
    }

    throw err;
}

} // namespace pfs
