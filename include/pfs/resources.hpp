////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2024 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2024.07.21 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "error.hpp"
#include "i18n.hpp"
#include <cstdint>
#include <map>
#include <string>

namespace pfs {

class resources
{
public:
    struct item
    {
        std::uint8_t const * data;
        std::size_t length;
    };

private:
    std::map<std::string, item> _data;

public:
    resources () = default;

public:
    resources (resources const &) = delete;
    resources (resources &&) = delete;
    resources & operator = (resources const &) = delete;
    resources & operator = (resources &&) = delete;

public:
    bool contains (std::string const & alias)
    {
        return _data.find(alias) != _data.end();
    }

    void add (std::string alias, std::uint8_t const * data, std::size_t length)
    {
        auto result = _data.emplace(std::move(alias), item {data, length});

        if (!result.second) {
            throw error {
                  std::make_error_code(std::errc::file_exists)
                , tr::f_("resource already exists by alias: {}", alias)
            };
        }
    }

    item const * get (std::string const & alias)
    {
        auto pos = _data.find(alias);

        if (pos == _data.end())
            return nullptr;

        return & pos->second;
    }

// @deprecated
//public: // static
//    static resources & instance ()
//    {
//        static resources r;
//        return r;
//    }
};

} // namespace pfs
