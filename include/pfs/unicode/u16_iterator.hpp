////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of [pfs-common](https://github.com/semenovf/pfs-common) library.
//
// Changelog:
//      2020.11.01 Initial version
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <pfs/unicode/ux_iterator.hpp>

namespace pfs {
namespace unicode {

template <typename HextetInputIt, typename BrokenSeqAction = ignore_broken_sequence>
class u16_input_iterator
    : public details::ux_input_iterator<u16_input_iterator<HextetInputIt, BrokenSeqAction>
        , HextetInputIt, BrokenSeqAction>
{
    using base_class = details::ux_input_iterator<u16_input_iterator
        , HextetInputIt, BrokenSeqAction>;

public:
    using difference_type = typename base_class::difference_type;

public:
    using base_class::base_class;

//     void increment (difference_type)
//     {
//         // TODO Implement (see u8_input_iterator::increment)
//     }
};

}} // pfs::unicode
