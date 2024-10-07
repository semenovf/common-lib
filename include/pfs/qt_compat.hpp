////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2022 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2022.04.11 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "fmt.hpp"
#include <QString>

namespace fmt {

template <>
struct formatter<QString>
{
    template <typename ParseContext>
    constexpr auto parse (ParseContext & ctx) const -> decltype(ctx.begin())
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format (QString const & s, FormatContext & ctx) const -> decltype(ctx.out())
    {
        return format_to(ctx.out(), "{}", s.toStdString());
    }
};

} // namespace fmt

#include <QByteArray>
#include <QHash>

namespace std {

inline unsigned int qHash (std::string const & key, unsigned int seed = 0)
{
    return qHash(QByteArray::fromRawData(key.data(), static_cast<int>(key.length())), seed);
}

} // namespace std
