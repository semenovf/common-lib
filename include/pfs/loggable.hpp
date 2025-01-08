////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2025.01.08 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "namespace.hpp"
#include <functional>
#include <string>

PFS__NAMESPACE_BEGIN

class loggable
{
private:
    static void null_log (std::string const &) {}

public:
    void (* on_verbose) (std::string const &) = & loggable::null_log;
    void (* on_debug) (std::string const &) = & loggable::null_log;
    void (* on_info) (std::string const &) = & loggable::null_log;
    void (* on_warn) (std::string const &) = & loggable::null_log;
    void (* on_error) (std::string const &) = & loggable::null_log;
};

class functional_loggable
{
public:
    std::function<void (std::string const &)> on_verbose = [] (std::string const &) {};
    std::function<void (std::string const &)> on_debug = [] (std::string const &) {};
    std::function<void (std::string const &)> on_info = [] (std::string const &) {};
    std::function<void (std::string const &)> on_warn = [] (std::string const &) {};
    std::function<void (std::string const &)> on_error = [] (std::string const &) {};
};

PFS__NAMESPACE_END
