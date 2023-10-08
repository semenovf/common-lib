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

/**
 * Simple Command Line Parser API
 */
class argvapi
{
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
        string_view _optname;
        string_view _arg;

    private:
        item (type_enum type, string_view optname, string_view arg)
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

        bool is_option (string_view optname) const noexcept
        {
            return (is_option() && _optname == optname);
        }

        bool has_arg () const noexcept
        {
            return !_arg.empty();
        }

        string_view optname () const noexcept
        {
            return _optname;
        }

        string_view arg () const noexcept
        {
            return _arg;
        }
    };

    class iterator
    {
    private:
        int _argc {0};
        char const * const * _argv {nullptr};

    private:
        friend class argvapi;

        iterator (int argc, char const * const * argv)
            : _argc(argc)
            , _argv(argv)
        {}

    public:
        iterator () = default;
        iterator (iterator const & other) = default;
        iterator & operator = (iterator const & other) = default;
        ~iterator () = default;

        iterator (iterator && other)
            : _argc(other._argc)
            , _argv(other._argv)
        {
            other._argc = 0;
            other._argv = nullptr;
        }

        iterator & operator = (iterator && other)
        {
            _argc = other._argc;
            _argv = other._argv;
            other._argc = 0;
            other._argv = nullptr;
            return *this;
        }

        /**
         * Returns @c true if has more unread options/arguments, @c false otherwise.
         */
        bool has_more () const noexcept
        {
            return _argc > 0;
        }

        /**
         * Iterate to the next item.
         */
        item next ()
        {
            if (_argc == 0)
                return item{type_enum::none, string_view{}, string_view{}};

            if (_argv == nullptr)
                throw make_exception(errc::unexpected_error);

            string_view arg{*_argv};
            string_view optname;
            string_view optarg;
            auto type = type_enum::none;

            if (pfs::starts_with(arg, "-")) {
                if (arg == "--") {
                    type = type_enum::doubledash;
                } else if (arg == "-") {
                    type = type_enum::singledash;
                } else {
                    auto first = arg.begin();
                    ++first; // Skip first dash;

                    if (*first == '-') {
                        type = type_enum::longstyleopt;
                        ++first;
                    } else {
                        type = type_enum::shortstyleopt;
                    }

                    auto last = std::find(first, arg.end(), '=');
                    optname = string_view {first, static_cast<std::size_t>(std::distance(first, last))};

                    if (last != arg.end()) {
                        ++last;
                        optarg = string_view {last, static_cast<std::size_t>(std::distance(last, arg.end()))};
                    }
                }
            } else {
                optarg = arg;
            }

            _argc--;
            _argv++;

            return item{type, optname, optarg};
        }
    };

private:
    int _argc {0};
    char const * const * _argv {nullptr};
    filesystem::path _program;
    filesystem::path _program_name;

public:
    argvapi () = delete;
    argvapi (argvapi const & other) = delete;
    argvapi (argvapi && other) = delete;
    argvapi & operator = (argvapi const & other) = delete;
    argvapi & operator = (argvapi && other) = delete;

    argvapi (int argc, char const * const * argv, bool program_name_skipped = false)
        : _argc(argc)
        , _argv(argv)
    {
        if (_argc < 1) {
            throw error {
                  std::make_error_code(std::errc::invalid_argument)
                , tr::_("expected at least one argument (program path) from command line")
            };
        }

        if (_argv == nullptr) {
            throw error {
                std::make_error_code(std::errc::invalid_argument)
                , tr::_("null command line argument list")
            };
        }

        if (!program_name_skipped) {
            _program = filesystem::canonical(filesystem::utf8_decode(argv[0]));
            _program_name = _program.filename();
            --_argc;
            ++_argv;
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
        return iterator{_argc, _argv};
    }
};

} // namespace pfs
