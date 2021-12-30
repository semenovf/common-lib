////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2021.12.29 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <string>
#include <system_error>
#include <cstdio>

namespace pfs {

namespace {
inline void assert_fail (char const * file, int line, char const * message)
{
    std::fprintf(stderr, "%s:%d: assertion failed: %s\n", file, line, message);
    std::terminate();
}
} // namespace

#ifndef PFS__ASSERT
#   ifdef NDEBUG
#       define PFS__ASSERT(condition, message)
#   else
#       define PFS__ASSERT(condition, message)                        \
            ((condition)                                              \
                ? (void)0                                             \
                : ::pfs::assert_fail(__FILE__, __LINE__, (message)))
#   endif
#endif

#ifndef PFS__THROW
#   if PFS__EXCEPTIONS_ENABLED
#       define PFS__THROW(x) throw x
#   else
#       define PFS__THROW(x)                                                \
            do {                                                            \
                ::pfs::assert_fail(__FILE__, __LINE__, (x).what().c_str()); \
            } while (false)
#   endif
#endif

#if PFS__EXCEPTIONS_ENABLED
// #   define PFS_TRY try
// #   define PFS_CATCH(x) catch (x)
#else
// #   define PFS_TRY if (true)
// #   define PFS_CATCH(x) if (false)
#endif

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

} // namespace pfs
