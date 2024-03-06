////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2024 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2024.03.06 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "error.hpp"
#include "filesystem.hpp"
#include "i18n.hpp"
#include <functional>

namespace pfs {
namespace filesystem {

class directory_scanner
{
public:
    std::function<void (pfs::filesystem::path const &)> on_entry
        = [] (pfs::filesystem::path const &) {};

public:
    directory_scanner () = default;
    directory_scanner (directory_scanner const &) = delete;
    directory_scanner (directory_scanner &&) = delete;
    directory_scanner & operator = (directory_scanner const &) = delete;
    directory_scanner & operator = (directory_scanner &&) = delete;
    ~directory_scanner () = default;

public:
    /**
     * Breadth-first traverse initial directory.
     */
    void scan (pfs::filesystem::path const & dir, error * perr = nullptr)
    {
        if (!pfs::filesystem::is_directory(dir)) {
            pfs::throw_or(perr, pfs::error {
                  make_error_code(std::errc::invalid_argument)
                , tr::f_("path is not a directory: {}", dir)
            });

            return;
        }

        std::error_code ec;
        pfs::filesystem::directory_iterator dir_iter{dir, ec};

        if (ec) {
            pfs::throw_or(perr, pfs::error {
                ec, tr::f_("directory iterator initialization failure: {}", dir)
            });

            return;
        }

        for (auto const & dir_entry: dir_iter)
            on_entry(dir_entry.path());
    }
};

}} // namespace pfs::filesystem

