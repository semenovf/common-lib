////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020-2023 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2023.06.28 Initial version.
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/numeric_cast.hpp"
#include <cstdint>

TEST_CASE("same integral types") {
    CHECK_EQ(pfs::numeric_cast<bool>(true), true);
    CHECK_EQ(pfs::numeric_cast<bool>(false), false);

    CHECK_EQ(pfs::numeric_cast<char>('x'), 'x');
    CHECK_EQ(pfs::numeric_cast<std::int8_t>(std::int8_t{-42}), std::int8_t{-42});
    CHECK_EQ(pfs::numeric_cast<std::uint8_t>(std::uint8_t{42}), std::uint8_t{42});
    CHECK_EQ(pfs::numeric_cast<std::int16_t>(std::int16_t{-42}), std::int16_t{-42});
    CHECK_EQ(pfs::numeric_cast<std::uint16_t>(std::uint16_t{42}), std::uint16_t{42});
    CHECK_EQ(pfs::numeric_cast<std::int32_t>(std::int32_t{-42}), std::int32_t{-42});
    CHECK_EQ(pfs::numeric_cast<std::uint32_t>(std::uint32_t{42}), std::uint32_t{42});
    CHECK_EQ(pfs::numeric_cast<std::int64_t>(std::int64_t{-42}), std::int64_t{-42});
    CHECK_EQ(pfs::numeric_cast<std::uint64_t>(std::uint64_t{42}), std::uint64_t{42});
    CHECK_EQ(pfs::numeric_cast<std::intmax_t>(std::intmax_t{-42}), std::intmax_t{-42});
    CHECK_EQ(pfs::numeric_cast<std::uintmax_t>(std::uintmax_t{42}), std::uintmax_t{42});
}

TEST_CASE("signed to signed integral types") {
    CHECK_THROWS_AS(pfs::numeric_cast<std::int8_t>(std::numeric_limits<std::int16_t>::max()), std::overflow_error);
    CHECK_THROWS_AS(pfs::numeric_cast<std::int8_t>(std::numeric_limits<std::int16_t>::min()), std::underflow_error);
    CHECK_THROWS_AS(pfs::numeric_cast<std::int8_t>(std::numeric_limits<std::int32_t>::max()), std::overflow_error);
    CHECK_THROWS_AS(pfs::numeric_cast<std::int8_t>(std::numeric_limits<std::int32_t>::min()), std::underflow_error);
    CHECK_THROWS_AS(pfs::numeric_cast<std::int8_t>(std::numeric_limits<std::int64_t>::max()), std::overflow_error);
    CHECK_THROWS_AS(pfs::numeric_cast<std::int8_t>(std::numeric_limits<std::int64_t>::min()), std::underflow_error);
    CHECK_EQ(pfs::numeric_cast<std::int8_t>(std::int16_t{42}), std::int8_t{42});
    CHECK_EQ(pfs::numeric_cast<std::int8_t>(std::int16_t{-42}), std::int8_t{-42});
    CHECK_EQ(pfs::numeric_cast<std::int8_t>(std::int32_t{42}), std::int8_t{42});
    CHECK_EQ(pfs::numeric_cast<std::int8_t>(std::int32_t{-42}), std::int8_t{-42});
    CHECK_EQ(pfs::numeric_cast<std::int8_t>(std::int64_t{42}), std::int8_t{42});
    CHECK_EQ(pfs::numeric_cast<std::int8_t>(std::int64_t{-42}), std::int8_t{-42});

    CHECK_EQ(pfs::numeric_cast<std::int16_t>(std::numeric_limits<std::int8_t>::max()), std::numeric_limits<std::int8_t>::max());
    CHECK_EQ(pfs::numeric_cast<std::int16_t>(std::numeric_limits<std::int8_t>::min()), std::numeric_limits<std::int8_t>::min());
    CHECK_THROWS_AS(pfs::numeric_cast<std::int16_t>(std::numeric_limits<std::int32_t>::max()), std::overflow_error);
    CHECK_THROWS_AS(pfs::numeric_cast<std::int16_t>(std::numeric_limits<std::int32_t>::min()), std::underflow_error);
    CHECK_THROWS_AS(pfs::numeric_cast<std::int16_t>(std::numeric_limits<std::int64_t>::max()), std::overflow_error);
    CHECK_THROWS_AS(pfs::numeric_cast<std::int16_t>(std::numeric_limits<std::int64_t>::min()), std::underflow_error);
    CHECK_EQ(pfs::numeric_cast<std::int16_t>(std::int8_t{42}), std::int16_t{42});
    CHECK_EQ(pfs::numeric_cast<std::int16_t>(std::int8_t{-42}), std::int16_t{-42});
    CHECK_EQ(pfs::numeric_cast<std::int16_t>(std::int32_t{42}), std::int16_t{42});
    CHECK_EQ(pfs::numeric_cast<std::int16_t>(std::int32_t{-42}), std::int16_t{-42});
    CHECK_EQ(pfs::numeric_cast<std::int16_t>(std::int64_t{42}), std::int16_t{42});
    CHECK_EQ(pfs::numeric_cast<std::int16_t>(std::int64_t{-42}), std::int16_t{-42});

    CHECK_EQ(pfs::numeric_cast<std::int32_t>(std::numeric_limits<std::int8_t>::max()), std::numeric_limits<std::int8_t>::max());
    CHECK_EQ(pfs::numeric_cast<std::int32_t>(std::numeric_limits<std::int8_t>::min()), std::numeric_limits<std::int8_t>::min());
    CHECK_EQ(pfs::numeric_cast<std::int32_t>(std::numeric_limits<std::int16_t>::max()), std::numeric_limits<std::int16_t>::max());
    CHECK_EQ(pfs::numeric_cast<std::int32_t>(std::numeric_limits<std::int16_t>::min()), std::numeric_limits<std::int16_t>::min());
    CHECK_THROWS_AS(pfs::numeric_cast<std::int32_t>(std::numeric_limits<std::int64_t>::max()), std::overflow_error);
    CHECK_THROWS_AS(pfs::numeric_cast<std::int32_t>(std::numeric_limits<std::int64_t>::min()), std::underflow_error);
    CHECK_EQ(pfs::numeric_cast<std::int32_t>(std::int8_t{42}), std::int32_t{42});
    CHECK_EQ(pfs::numeric_cast<std::int32_t>(std::int8_t{-42}), std::int32_t{-42});
    CHECK_EQ(pfs::numeric_cast<std::int32_t>(std::int16_t{42}), std::int32_t{42});
    CHECK_EQ(pfs::numeric_cast<std::int32_t>(std::int16_t{-42}), std::int32_t{-42});
    CHECK_EQ(pfs::numeric_cast<std::int32_t>(std::int64_t{42}), std::int32_t{42});
    CHECK_EQ(pfs::numeric_cast<std::int32_t>(std::int64_t{-42}), std::int32_t{-42});

    CHECK_EQ(pfs::numeric_cast<std::int64_t>(std::numeric_limits<std::int8_t>::max()), std::numeric_limits<std::int8_t>::max());
    CHECK_EQ(pfs::numeric_cast<std::int64_t>(std::numeric_limits<std::int8_t>::min()), std::numeric_limits<std::int8_t>::min());
    CHECK_EQ(pfs::numeric_cast<std::int64_t>(std::numeric_limits<std::int16_t>::max()), std::numeric_limits<std::int16_t>::max());
    CHECK_EQ(pfs::numeric_cast<std::int64_t>(std::numeric_limits<std::int16_t>::min()), std::numeric_limits<std::int16_t>::min());
    CHECK_EQ(pfs::numeric_cast<std::int64_t>(std::numeric_limits<std::int32_t>::max()), std::numeric_limits<std::int32_t>::max());
    CHECK_EQ(pfs::numeric_cast<std::int64_t>(std::numeric_limits<std::int32_t>::min()), std::numeric_limits<std::int32_t>::min());
    CHECK_EQ(pfs::numeric_cast<std::int64_t>(std::int8_t{42}), std::int64_t{42});
    CHECK_EQ(pfs::numeric_cast<std::int64_t>(std::int8_t{-42}), std::int64_t{-42});
    CHECK_EQ(pfs::numeric_cast<std::int64_t>(std::int16_t{42}), std::int64_t{42});
    CHECK_EQ(pfs::numeric_cast<std::int64_t>(std::int16_t{-42}), std::int64_t{-42});
    CHECK_EQ(pfs::numeric_cast<std::int64_t>(std::int32_t{42}), std::int64_t{42});
    CHECK_EQ(pfs::numeric_cast<std::int64_t>(std::int32_t{-42}), std::int64_t{-42});
}

TEST_CASE("unsigned to unsigned integral types") {
    CHECK_THROWS_AS(pfs::numeric_cast<std::uint8_t>(std::numeric_limits<std::uint16_t>::max()), std::overflow_error);
    CHECK_THROWS_AS(pfs::numeric_cast<std::uint8_t>(std::numeric_limits<std::uint32_t>::max()), std::overflow_error);
    CHECK_THROWS_AS(pfs::numeric_cast<std::uint8_t>(std::numeric_limits<std::uint64_t>::max()), std::overflow_error);
    CHECK_EQ(pfs::numeric_cast<std::uint8_t>(std::uint16_t{42}), std::uint8_t{42});
    CHECK_EQ(pfs::numeric_cast<std::uint8_t>(std::uint32_t{42}), std::uint8_t{42});
    CHECK_EQ(pfs::numeric_cast<std::uint8_t>(std::uint64_t{42}), std::uint8_t{42});

    CHECK_EQ(pfs::numeric_cast<std::uint16_t>(std::numeric_limits<std::uint8_t>::max()), std::numeric_limits<std::uint8_t>::max());
    CHECK_THROWS_AS(pfs::numeric_cast<std::uint16_t>(std::numeric_limits<std::uint32_t>::max()), std::overflow_error);
    CHECK_THROWS_AS(pfs::numeric_cast<std::uint16_t>(std::numeric_limits<std::uint64_t>::max()), std::overflow_error);
    CHECK_EQ(pfs::numeric_cast<std::uint16_t>(std::uint8_t{42}), std::uint8_t{42});
    CHECK_EQ(pfs::numeric_cast<std::uint16_t>(std::uint32_t{42}), std::uint16_t{42});
    CHECK_EQ(pfs::numeric_cast<std::uint16_t>(std::uint64_t{42}), std::uint16_t{42});

    CHECK_EQ(pfs::numeric_cast<std::uint32_t>(std::numeric_limits<std::uint8_t>::max()), std::numeric_limits<std::uint8_t>::max());
    CHECK_EQ(pfs::numeric_cast<std::uint32_t>(std::numeric_limits<std::uint16_t>::max()), std::numeric_limits<std::uint16_t>::max());
    CHECK_THROWS_AS(pfs::numeric_cast<std::uint32_t>(std::numeric_limits<std::uint64_t>::max()), std::overflow_error);
    CHECK_EQ(pfs::numeric_cast<std::uint32_t>(std::uint8_t{42}), std::uint32_t{42});
    CHECK_EQ(pfs::numeric_cast<std::uint32_t>(std::uint16_t{42}), std::uint32_t{42});
    CHECK_EQ(pfs::numeric_cast<std::uint32_t>(std::uint64_t{42}), std::uint32_t{42});
}

TEST_CASE("unsigned to signed integral types") {
    CHECK_THROWS_AS(pfs::numeric_cast<std::int8_t>(std::numeric_limits<std::uint8_t>::max()), std::overflow_error);
    CHECK_THROWS_AS(pfs::numeric_cast<std::int8_t>(std::numeric_limits<std::uint16_t>::max()), std::overflow_error);
    CHECK_THROWS_AS(pfs::numeric_cast<std::int8_t>(std::numeric_limits<std::uint32_t>::max()), std::overflow_error);
    CHECK_THROWS_AS(pfs::numeric_cast<std::int8_t>(std::numeric_limits<std::uint64_t>::max()), std::overflow_error);
    CHECK_EQ(pfs::numeric_cast<std::int8_t>(std::uint16_t{42}), std::int8_t{42});
    CHECK_EQ(pfs::numeric_cast<std::int8_t>(std::uint32_t{42}), std::int8_t{42});
    CHECK_EQ(pfs::numeric_cast<std::int8_t>(std::uint64_t{42}), std::int8_t{42});

    CHECK_EQ(pfs::numeric_cast<std::int16_t>(std::numeric_limits<std::uint8_t>::max()), std::numeric_limits<std::uint8_t>::max());
    CHECK_THROWS_AS(pfs::numeric_cast<std::int16_t>(std::numeric_limits<std::uint16_t>::max()), std::overflow_error);
    CHECK_THROWS_AS(pfs::numeric_cast<std::int16_t>(std::numeric_limits<std::uint32_t>::max()), std::overflow_error);
    CHECK_THROWS_AS(pfs::numeric_cast<std::int16_t>(std::numeric_limits<std::uint64_t>::max()), std::overflow_error);
    CHECK_EQ(pfs::numeric_cast<std::int16_t>(std::uint8_t{42}) , std::int16_t{42});
    CHECK_EQ(pfs::numeric_cast<std::int16_t>(std::uint32_t{42}), std::int16_t{42});
    CHECK_EQ(pfs::numeric_cast<std::int16_t>(std::uint64_t{42}), std::int16_t{42});

    CHECK_EQ(pfs::numeric_cast<std::int32_t>(std::numeric_limits<std::uint8_t>::max()), std::numeric_limits<std::uint8_t>::max());
    CHECK_EQ(pfs::numeric_cast<std::int32_t>(std::numeric_limits<std::uint16_t>::max()), std::numeric_limits<std::uint16_t>::max());
    CHECK_THROWS_AS(pfs::numeric_cast<std::int32_t>(std::numeric_limits<std::uint32_t>::max()), std::overflow_error);
    CHECK_THROWS_AS(pfs::numeric_cast<std::int32_t>(std::numeric_limits<std::uint64_t>::max()), std::overflow_error);
    CHECK_EQ(pfs::numeric_cast<std::int32_t>(std::uint8_t{42}) , std::int32_t{42});
    CHECK_EQ(pfs::numeric_cast<std::int32_t>(std::uint16_t{42}), std::int32_t{42});
    CHECK_EQ(pfs::numeric_cast<std::int32_t>(std::uint64_t{42}), std::int32_t{42});

    CHECK_EQ(pfs::numeric_cast<std::int64_t>(std::numeric_limits<std::uint8_t>::max()), std::numeric_limits<std::uint8_t>::max());
    CHECK_EQ(pfs::numeric_cast<std::int64_t>(std::numeric_limits<std::uint16_t>::max()), std::numeric_limits<std::uint16_t>::max());
    CHECK_EQ(pfs::numeric_cast<std::int64_t>(std::numeric_limits<std::uint32_t>::max()), std::numeric_limits<std::uint32_t>::max());
    CHECK_THROWS_AS(pfs::numeric_cast<std::int64_t>(std::numeric_limits<std::uint64_t>::max()), std::overflow_error);
    CHECK_EQ(pfs::numeric_cast<std::int64_t>(std::uint8_t{42}) , std::int64_t{42});
    CHECK_EQ(pfs::numeric_cast<std::int64_t>(std::uint16_t{42}), std::int64_t{42});
    CHECK_EQ(pfs::numeric_cast<std::int64_t>(std::uint32_t{42}), std::int64_t{42});
}

TEST_CASE("signed to unsigned integral types") {
    CHECK_EQ(pfs::numeric_cast<std::uint8_t>(std::numeric_limits<std::int8_t>::max()), std::numeric_limits<std::int8_t>::max());
    CHECK_THROWS_AS(pfs::numeric_cast<std::uint8_t>(std::numeric_limits<std::int8_t>::min()), std::underflow_error);
    CHECK_THROWS_AS(pfs::numeric_cast<std::uint8_t>(std::numeric_limits<std::int16_t>::max()), std::overflow_error);
    CHECK_THROWS_AS(pfs::numeric_cast<std::uint8_t>(std::numeric_limits<std::int16_t>::min()), std::underflow_error);
    CHECK_THROWS_AS(pfs::numeric_cast<std::uint8_t>(std::numeric_limits<std::int32_t>::max()), std::overflow_error);
    CHECK_THROWS_AS(pfs::numeric_cast<std::uint8_t>(std::numeric_limits<std::int32_t>::min()), std::underflow_error);
    CHECK_THROWS_AS(pfs::numeric_cast<std::uint8_t>(std::numeric_limits<std::int64_t>::max()), std::overflow_error);
    CHECK_THROWS_AS(pfs::numeric_cast<std::uint8_t>(std::numeric_limits<std::int64_t>::min()), std::underflow_error);

    CHECK_EQ(pfs::numeric_cast<std::uint16_t>(std::numeric_limits<std::int8_t>::max()), std::numeric_limits<std::int8_t>::max());
    CHECK_THROWS_AS(pfs::numeric_cast<std::uint16_t>(std::numeric_limits<std::int8_t>::min()), std::underflow_error);
    CHECK_EQ(pfs::numeric_cast<std::uint16_t>(std::numeric_limits<std::int16_t>::max()), std::numeric_limits<std::int16_t>::max());
    CHECK_THROWS_AS(pfs::numeric_cast<std::uint16_t>(std::numeric_limits<std::int16_t>::min()), std::underflow_error);
    CHECK_THROWS_AS(pfs::numeric_cast<std::uint16_t>(std::numeric_limits<std::int32_t>::max()), std::overflow_error);
    CHECK_THROWS_AS(pfs::numeric_cast<std::uint16_t>(std::numeric_limits<std::int32_t>::min()), std::underflow_error);
    CHECK_THROWS_AS(pfs::numeric_cast<std::uint16_t>(std::numeric_limits<std::int64_t>::max()), std::overflow_error);
    CHECK_THROWS_AS(pfs::numeric_cast<std::uint16_t>(std::numeric_limits<std::int64_t>::min()), std::underflow_error);

    CHECK_EQ(pfs::numeric_cast<std::uint32_t>(std::numeric_limits<std::int8_t>::max()), std::numeric_limits<std::int8_t>::max());
    CHECK_THROWS_AS(pfs::numeric_cast<std::uint32_t>(std::numeric_limits<std::int8_t>::min()), std::underflow_error);
    CHECK_EQ(pfs::numeric_cast<std::uint32_t>(std::numeric_limits<std::int16_t>::max()), std::numeric_limits<std::int16_t>::max());
    CHECK_THROWS_AS(pfs::numeric_cast<std::uint32_t>(std::numeric_limits<std::int16_t>::min()), std::underflow_error);
    CHECK_EQ(pfs::numeric_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max()), std::numeric_limits<std::int32_t>::max());
    CHECK_THROWS_AS(pfs::numeric_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::min()), std::underflow_error);
    CHECK_THROWS_AS(pfs::numeric_cast<std::uint32_t>(std::numeric_limits<std::int64_t>::max()), std::overflow_error);
    CHECK_THROWS_AS(pfs::numeric_cast<std::uint32_t>(std::numeric_limits<std::int64_t>::min()), std::underflow_error);

    CHECK_EQ(pfs::numeric_cast<std::uint64_t>(std::numeric_limits<std::int8_t>::max()), std::numeric_limits<std::int8_t>::max());
    CHECK_THROWS_AS(pfs::numeric_cast<std::uint64_t>(std::numeric_limits<std::int8_t>::min()), std::underflow_error);
    CHECK_EQ(pfs::numeric_cast<std::uint64_t>(std::numeric_limits<std::int16_t>::max()), std::numeric_limits<std::int16_t>::max());
    CHECK_THROWS_AS(pfs::numeric_cast<std::uint64_t>(std::numeric_limits<std::int16_t>::min()), std::underflow_error);
    CHECK_EQ(pfs::numeric_cast<std::uint64_t>(std::numeric_limits<std::int32_t>::max()), std::numeric_limits<std::int32_t>::max());
    CHECK_THROWS_AS(pfs::numeric_cast<std::uint64_t>(std::numeric_limits<std::int32_t>::min()), std::underflow_error);
    CHECK_EQ(pfs::numeric_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()), std::numeric_limits<std::int64_t>::max());
    CHECK_THROWS_AS(pfs::numeric_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::min()), std::underflow_error);
}

TEST_CASE("boolean to signed integral types") {
    CHECK_EQ(pfs::numeric_cast<std::int8_t>(true), static_cast<std::int8_t>(true));
    CHECK_EQ(pfs::numeric_cast<std::int8_t>(false), static_cast<std::int8_t>(false));
    CHECK_EQ(pfs::numeric_cast<std::int32_t>(true), static_cast<std::int32_t>(true));
    CHECK_EQ(pfs::numeric_cast<std::int32_t>(false), static_cast<std::int32_t>(false));
}

TEST_CASE("boolean to unsigned integral types") {
    CHECK_EQ(pfs::numeric_cast<std::uint8_t>(true), static_cast<std::uint8_t>(true));
    CHECK_EQ(pfs::numeric_cast<std::uint8_t>(false), static_cast<std::uint8_t>(false));
    CHECK_EQ(pfs::numeric_cast<std::uint32_t>(true), static_cast<std::uint32_t>(true));
    CHECK_EQ(pfs::numeric_cast<std::uint32_t>(false), static_cast<std::uint32_t>(false));
}

TEST_CASE("char to signed integral types") {
    CHECK_EQ(pfs::numeric_cast<std::int8_t>('*'), 42);
    CHECK_EQ(pfs::numeric_cast<std::int32_t>('*'), 42);
}

TEST_CASE("char to unsigned integral types") {
    CHECK_EQ(pfs::numeric_cast<std::uint8_t>('*'), 42);
    CHECK_EQ(pfs::numeric_cast<std::uint32_t>('*'), 42);
}
