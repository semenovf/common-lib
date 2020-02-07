#pragma once
#include <pfs/exception.hpp>

namespace pfs {
namespace unicode {

struct except_broken_sequence
{
    void operator () () const { PFS_THROW(runtime_error("broken utf-8 sequence")); }
};

struct ignore_broken_sequence
{
    void operator () () const {}
};

}} // pfs::unicode
