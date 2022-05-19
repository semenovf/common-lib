////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020-2022 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2022.05.19 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <random>

namespace datagen {
namespace random {

using random_engine_t = std::mt19937;

inline auto engine () -> random_engine_t
{
    static std::random_device __rd; // Will be used to obtain a seed for the random number engine
    return random_engine_t{__rd()}; // Standard mersenne_twister_engine seeded with rd()
}

inline auto integer (int from, int to) -> int
{
    std::uniform_int_distribution<int> distrib{from, to + 1};
    auto rnd = engine();
    return distrib(rnd);
}

inline auto index (int size) -> int
{
    std::uniform_int_distribution<int> distrib{0, size - 1};
    auto rnd = engine();
    return distrib(rnd);
}

inline auto boolean () -> int
{
    return !!integer(0, 1);
}

inline auto angle () -> double
{
    static std::uniform_real_distribution<double> __distrib_angle{0, 360.0f};
    auto rnd = engine();
    return __distrib_angle(rnd);
}

}} // namespace datagen::random
