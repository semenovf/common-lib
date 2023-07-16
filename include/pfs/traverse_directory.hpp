////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020-2023 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2023.07.16 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "error.hpp"
#include "filesystem.hpp"
#include "i18n.hpp"
#include <functional>
#include <stack>

namespace pfs {
namespace filesystem {

// See File tree work also (man 3 ftw)

class directory_traversal
{
public:
    std::function<void (pfs::filesystem::path const &)> on_enter_directory
        = [] (pfs::filesystem::path const &) {};

    std::function<void (pfs::filesystem::path const &)> on_leave_directory
        = [] (pfs::filesystem::path const &) {};

    std::function<void (pfs::filesystem::path const &)> on_entry
        = [] (pfs::filesystem::path const &) {};

    std::function<void (pfs::filesystem::path const &, error const &)> on_entry_error
        = [] (pfs::filesystem::path const &, error const &) {};

    std::function<void (error const &)> on_error = [] (error const &) {};

public:
    directory_traversal () = default;
    directory_traversal (directory_traversal const &) = delete;
    directory_traversal (directory_traversal &&) = delete;
    directory_traversal & operator = (directory_traversal const &) = delete;
    directory_traversal & operator = (directory_traversal &&) = delete;
    ~directory_traversal () = default;

public:
    /**
     * Breadth-first traverse initial directory.
     */
    void traverse (pfs::filesystem::path const & initial_dir)
    {
        if (!pfs::filesystem::is_directory(initial_dir)) {
            on_entry_error(initial_dir, pfs::error {
                std::make_error_code(std::errc::invalid_argument)
                , tr::_("initial path is not a directory")
            });
            return;
        }

        std::stack<std::pair<pfs::filesystem::path, bool>> dirs;
        dirs.push(std::make_pair(initial_dir, false));

        while (!dirs.empty()) {
            auto & top = dirs.top();

            if (top.second) {
                on_leave_directory(top.first);
                dirs.pop();
                continue;
            }

            std::error_code ec;
            pfs::filesystem::directory_iterator dir_iter{top.first, ec};

            if (ec) {
                on_entry_error(top.first, pfs::error {ec});
                dirs.pop();
                continue;
            }

            on_enter_directory(top.first);

            for (auto const & dir_entry: dir_iter) {
                if (dir_entry.is_directory()) {
                    dirs.push(std::make_pair(dir_entry.path(), false));
                } else {
                    on_entry(dir_entry.path());
                }
            }

            top.second = true;
        }
    }
};

}} // namespace pfs::filesystem
