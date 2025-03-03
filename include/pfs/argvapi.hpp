////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2023 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2023.10.01 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "error.hpp"
#include "filesystem.hpp"
#include "i18n.hpp"
#include "string_view.hpp"
#include <iterator>

namespace pfs {

template <typename CharT = char>
struct argvapi_traits;

template <>
struct argvapi_traits <char>
{
    static constexpr char const * singledash_str  = "-";
    static constexpr char const * doubledash_str  = "--";
    static constexpr char singledash_char  = '-';
    static constexpr char assign_char  = '=';
};

template <>
struct argvapi_traits <wchar_t>
{
    static constexpr wchar_t const * singledash_str  = L"-";
    static constexpr wchar_t const * doubledash_str  = L"--";
    static constexpr wchar_t singledash_char  = L'-';
    static constexpr wchar_t assign_char  = L'=';
};

/**
 * Simple Command Line Parser API
 */
template <typename ArgvFwdIter, typename CharT = char>
class argvapi
{
    using string_view_type = basic_string_view<CharT>;

private:
    enum class type_enum
    {
          none
        , longstyleopt   // --long-opt[=arg]
        , shortstyleopt  // -o | -opt[=arg]
        , winstyleopt    // /opt[:arg]
        , doubledash     // --
        , singledash     // -
        , arg
    };

public:
    class iterator;

    class item
    {
    private:
        friend class iterator;

    private:
        type_enum _type {type_enum::none};
        string_view_type _optname;
        string_view_type _arg;

    private:
        item (type_enum type, string_view_type optname, string_view_type arg)
            : _type(type)
            , _optname(optname)
            , _arg(arg)
        {}

    public:
        bool is_double_dash () const noexcept
        {
            return _type == type_enum::doubledash;
        }

        bool is_single_dash () const noexcept
        {
            return _type == type_enum::singledash;
        }

        bool is_option () const noexcept
        {
            return (_type == type_enum::longstyleopt
                || _type == type_enum::shortstyleopt
                || _type == type_enum::winstyleopt);
        }

        bool is_option (string_view_type optname) const noexcept
        {
            return (is_option() && _optname == optname);
        }

        bool is_arg () const noexcept
        {
            return _type == type_enum::arg;
        }

        bool has_arg () const noexcept
        {
            return !_arg.empty();
        }

        string_view_type optname () const noexcept
        {
            return _optname;
        }

        string_view_type arg () const noexcept
        {
            return _arg;
        }
    };

    class iterator
    {
    private:
        ArgvFwdIter _pos;
        ArgvFwdIter _end;

    private:
        friend class argvapi;

        iterator (ArgvFwdIter begin, ArgvFwdIter end)
            : _pos(begin)
            , _end(end)
        {}

    public:
        /**
         * Returns @c true if has more unread options/arguments, @c false otherwise.
         */
        bool has_more () const noexcept
        {
            return _pos != _end;
        }

        /**
         * Iterate to the next item.
         */
        item next ()
        {
            if (_pos == _end)
                return item{type_enum::none, string_view_type{}, string_view_type{}};

            string_view_type arg{*_pos};
            string_view_type optname;
            string_view_type optarg;
            auto type = type_enum::none;

            if (pfs::starts_with(arg, argvapi_traits<CharT>::singledash_str)) { // "-"
                if (arg == argvapi_traits<CharT>::doubledash_str) { // "--"
                    type = type_enum::doubledash;
                } else if (arg == argvapi_traits<CharT>::singledash_str) { // "-"
                    type = type_enum::singledash;
                } else {
                    auto first = arg.begin();
                    ++first; // Skip first dash;

                    if (*first == argvapi_traits<CharT>::singledash_char) { // '-'
                        type = type_enum::longstyleopt;
                        ++first;
                    } else {
                        type = type_enum::shortstyleopt;
                    }

                    auto last = std::find(first, arg.end(), argvapi_traits<CharT>::assign_char); // '='
                    optname = string_view_type {first, static_cast<std::size_t>(std::distance(first, last))};

                    if (last != arg.end()) {
                        ++last;
                        optarg = string_view_type {last, static_cast<std::size_t>(std::distance(last, arg.end()))};
                    }
                }
            } else {
                type = type_enum::arg;
                optarg = arg;
            }

            ++_pos;

            return item{type, optname, optarg};
        }
    };

private:
    ArgvFwdIter _pos;
    ArgvFwdIter _end;
    filesystem::path _program;
    filesystem::path _program_name;

public:
    argvapi (ArgvFwdIter begin, ArgvFwdIter end, bool program_name_skipped = false)
        : _pos(begin)
        , _end(end)
    {
        if (_pos == _end)
            return;

        if (!program_name_skipped) {
            //_program = filesystem::canonical(filesystem::utf8_decode(*_pos));
            _program = filesystem::canonical(*_pos);
            _program_name = _program.filename();
            ++_pos;
        }
    }

    ~argvapi () = default;

    /**
     * Program canonical absolute path.
     */
    filesystem::path const & program () const noexcept
    {
        return _program;
    }

    /**
     * Program file name.
     */
    filesystem::path const & program_name () const noexcept
    {
        return _program_name;
    }

    iterator begin () const noexcept
    {
        return iterator{_pos, _end};
    }
};

template <typename ArgvFwdIter, typename CharT = char>
inline argvapi<ArgvFwdIter> make_argvapi (ArgvFwdIter begin, ArgvFwdIter end
    , bool program_name_skipped = false)
{
    return argvapi<ArgvFwdIter>{begin, end, program_name_skipped};
}

inline argvapi<char const * const *, char> make_argvapi (int argc, char const * const * argv
    , bool program_name_skipped = false)
{
    return argvapi<char const * const *, char>{argv, argv + argc, program_name_skipped};
}

#if _MSC_VER
inline argvapi<wchar_t const * const *, wchar_t> make_argvapi (int argc, wchar_t const * const * argv
    , bool program_name_skipped = false)
{
    return argvapi<wchar_t const * const *, wchar_t>{argv, argv + argc, program_name_skipped};
}
#endif

constexpr char const * argvapi_traits<char>::singledash_str;
constexpr char const * argvapi_traits<char>::doubledash_str;
constexpr char argvapi_traits<char>::singledash_char;
constexpr char argvapi_traits<char>::assign_char;

constexpr wchar_t const * argvapi_traits <wchar_t>::singledash_str;
constexpr wchar_t const * argvapi_traits <wchar_t>::doubledash_str;
constexpr wchar_t argvapi_traits <wchar_t>::singledash_char;
constexpr wchar_t argvapi_traits <wchar_t>::assign_char;

} // namespace pfs
