////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2022 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2022.06.28 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "string_view.hpp"
#include <cctype>
#include <map>

namespace pfs {

namespace locale_utils_details {

template <typename ForwardIt>
inline void skip_ws (ForwardIt & pos, ForwardIt end)
{
    while (pos != end && std::isspace(*pos))
        ++pos;
}

} // namespace locale_utils_details

/**
 * Parses string returned by std::locale("").
 */
inline
std::map<std::string, std::string>
parse_locale_name (std::string const & locname)
{
    string_view sv {locname};
    std::map<std::string, std::string> result;

    if (starts_with(sv, "LC_")) {
        auto pos = sv.begin();

        while (pos != sv.end()) {
            locale_utils_details::skip_ws(pos, sv.end());

            auto cat_name_first = pos;

            // Read category
            while (pos != sv.end() && *pos != '=')
                ++pos;

            auto cat_name_last = pos;

            // Skip 'assign' character
            if (pos != sv.end())
                ++pos;

            auto cat_val_first = pos;

            while (pos != sv.end() && *pos != ';')
                ++pos;

            auto cat_val_last = pos;

            locale_utils_details::skip_ws(pos, sv.end());

            // Skip semicolon character
            if (pos != sv.end())
                ++pos;

            result.emplace(
                  std::string{cat_name_first, cat_name_last}
                , std::string{cat_val_first, cat_val_last});
        }
    }

    return result;
}

struct locale_context
{
    std::string lang;
    std::string zone;
    std::string encoding;
    std::string mod;
};

/**
 * Parses category value.
 *
 * @details Parser recognizes @a catval in format:
 *      lang['_' ZONE['.' enc]]['@' modifier].
 *
 */
inline locale_context parse_locale_category (std::string const & catval)
{
    string_view sv {catval};
    locale_context result;

    auto pos = sv.begin();

    while (pos != sv.end()) {
        auto lang_first = pos;

        // Read language
        while (pos != sv.end() && *pos != '_')
            ++pos;

        auto lang_last = pos;

        // Skip 'underscore' character
        if (pos != sv.end())
            ++pos;

        auto zone_first = pos;

        while (pos != sv.end() && *pos != '.')
            ++pos;

        auto zone_last = pos;

        // Skip point
        if (pos != sv.end())
            ++pos;

        auto enc_first = pos;

        while (pos != sv.end() && *pos != '@')
            ++pos;

        auto enc_last = pos;

        // Skip `@` character
        if (pos != sv.end())
            ++pos;

        auto mod_first = pos;

        while (pos != sv.end())
            ++pos;

        auto mod_last = pos;

        return locale_context {
              std::string{lang_first, lang_last}
            , std::string{zone_first, zone_last}
            , std::string{enc_first , enc_last}
            , std::string{mod_first , mod_last}
        };
    }

    return locale_context{};
}

} // namespace pfs
