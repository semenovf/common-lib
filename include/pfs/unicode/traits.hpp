#pragma once
#include <stdexcept>

namespace pfs {
namespace unicode {

struct except_broken_sequence
{
    void operator () () const { throw std::runtime_error("broken utf-8 sequence"); }
};

struct ignore_broken_sequence
{
    void operator () () const {}
};

}} // pfs::unicode
