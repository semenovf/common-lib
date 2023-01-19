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
    , unexpected_error // Replaces any unexpected error
};

class error_category : public std::error_category
{
public:
    virtual char const * name () const noexcept override
    {
        return "pfs_common_category";
    }

    virtual std::string message (int ev) const override
    {
        switch (ev) {
            case static_cast<int>(errc::success):
                return std::string{"no error"};
            case static_cast<int>(errc::unexpected_error):
                return std::string{"Unexpected error"};

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
class error
{
private:
    std::error_code _ec;
    // FIXME Need separately-allocated reference-counted string representation
    // for this members.
    std::string _description;
    std::string _cause;

public:
    error () = default;

    error (std::error_code ec)
        : _ec(ec)
    {}

    error (std::error_code ec
        , std::string const & description
        , std::string const & cause)
        : _ec(ec)
        , _description(description)
        , _cause(cause)
    {}

    error (std::error_code ec
        , std::string const & description)
        : _ec(ec)
        , _description(description)
    {}

    // FIXME Need separately-allocated reference-counted string representation
    // for internal members.
    // See https://en.cppreference.com/w/cpp/error/runtime_error
    // "Because copying std::runtime_error is not permitted to throw exceptions,
    // this message is typically stored internally as a separately-allocated
    // reference-counted string. This is also why there is no constructor
    // taking std::string&&: it would have to copy the content anyway."

    error (error const & other ) /*noexcept*/ = default;
    error (error && other ) /*noexcept*/ = default;
    error & operator = (error const & other ) /*noexcept*/ = default;
    error & operator = (error && other ) /*noexcept*/ = default;

    operator bool () const noexcept
    {
        return !!_ec;
    }

    std::error_code code () const noexcept
    {
        return _ec;
    }

    std::string error_message () const noexcept
    {
        return _ec.message();
    }

    std::string const & description () const noexcept
    {
        return _description;
    }

    std::string const & cause () const noexcept
    {
        return _cause;
    }

    std::string what () const noexcept
    {
        std::string result;

        if (_ec)
            result += _ec.message();

        if (!_description.empty())
            result += std::string{(result.empty() ? "" : ": ")} + _description;

        if (!_cause.empty())
            result += std::string{(result.empty() ? "" : " ")} + '(' + _cause + ')';

        return result;
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

} // namespace pfs
