////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2021.12.03 Initial version (from tl::expected tests).
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/expected.hpp"
#include <type_traits>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

struct takes_init_and_variadic {
    std::vector<int> v;
    std::tuple<int, int> t;
    template <class... Args>
    takes_init_and_variadic(std::initializer_list<int> l, Args &&... args)
        : v(l), t(std::forward<Args>(args)...) {}
};

TEST_CASE("Constructors") {
    {
        pfs::expected<int,int> e;
        REQUIRE(e);
        REQUIRE(e == 0);
    }

    {
        pfs::expected<int,int> e = pfs::make_unexpected(0);
        REQUIRE(!e);
        REQUIRE(e.error() == 0);
    }

    {
        pfs::expected<int,int> e (pfs::unexpect, 0);
        REQUIRE(!e);
        REQUIRE(e.error() == 0);
    }

    {
        pfs::expected<int,int> e (pfs::in_place, 42);
        REQUIRE(e);
        REQUIRE(e == 42);
    }

    {
        pfs::expected<std::vector<int>,int> e (pfs::in_place, {0,1});
        REQUIRE(e);
        REQUIRE((*e)[0] == 0);
        REQUIRE((*e)[1] == 1);
    }

    {
        pfs::expected<std::tuple<int,int>,int> e (pfs::in_place, 0, 1);
        REQUIRE(e);
        REQUIRE(std::get<0>(*e) == 0);
        REQUIRE(std::get<1>(*e) == 1);
    }

    {
        pfs::expected<takes_init_and_variadic,int> e (pfs::in_place, {0,1}, 2, 3);
        REQUIRE(e);
        REQUIRE(e->v[0] == 0);
        REQUIRE(e->v[1] == 1);
        REQUIRE(std::get<0>(e->t) == 2);
        REQUIRE(std::get<1>(e->t) == 3);
    }

    {
        pfs::expected<int, int> e;
        REQUIRE(std::is_default_constructible<decltype(e)>::value);
        REQUIRE(std::is_copy_constructible<decltype(e)>::value);
        REQUIRE(std::is_move_constructible<decltype(e)>::value);
        REQUIRE(std::is_copy_assignable<decltype(e)>::value);
        REQUIRE(std::is_move_assignable<decltype(e)>::value);
        REQUIRE(TL_EXPECTED_IS_TRIVIALLY_COPY_CONSTRUCTIBLE(decltype(e))::value);
        REQUIRE(TL_EXPECTED_IS_TRIVIALLY_COPY_ASSIGNABLE(decltype(e))::value);
#   if !defined(TL_EXPECTED_GCC49)
        REQUIRE(std::is_trivially_move_constructible<decltype(e)>::value);
        REQUIRE(std::is_trivially_move_assignable<decltype(e)>::value);
#   endif
    }

    {
        pfs::expected<int, std::string> e;
        REQUIRE(std::is_default_constructible<decltype(e)>::value);
        REQUIRE(std::is_copy_constructible<decltype(e)>::value);
        REQUIRE(std::is_move_constructible<decltype(e)>::value);
        REQUIRE(std::is_copy_assignable<decltype(e)>::value);
        REQUIRE(std::is_move_assignable<decltype(e)>::value);
        REQUIRE(!TL_EXPECTED_IS_TRIVIALLY_COPY_CONSTRUCTIBLE(decltype(e))::value);
        REQUIRE(!TL_EXPECTED_IS_TRIVIALLY_COPY_ASSIGNABLE(decltype(e))::value);
#if !defined(TL_EXPECTED_GCC49)
        REQUIRE(!std::is_trivially_move_constructible<decltype(e)>::value);
        REQUIRE(!std::is_trivially_move_assignable<decltype(e)>::value);
#endif
    }

    {
        pfs::expected<std::string, int> e;
        REQUIRE(std::is_default_constructible<decltype(e)>::value);
        REQUIRE(std::is_copy_constructible<decltype(e)>::value);
        REQUIRE(std::is_move_constructible<decltype(e)>::value);
        REQUIRE(std::is_copy_assignable<decltype(e)>::value);
        REQUIRE(std::is_move_assignable<decltype(e)>::value);
        REQUIRE(!TL_EXPECTED_IS_TRIVIALLY_COPY_CONSTRUCTIBLE(decltype(e))::value);
        REQUIRE(!TL_EXPECTED_IS_TRIVIALLY_COPY_ASSIGNABLE(decltype(e))::value);
#if !defined(TL_EXPECTED_GCC49)
        REQUIRE(!std::is_trivially_move_constructible<decltype(e)>::value);
        REQUIRE(!std::is_trivially_move_assignable<decltype(e)>::value);
#endif
    }

    {
        pfs::expected<std::string, std::string> e;
        REQUIRE(std::is_default_constructible<decltype(e)>::value);
        REQUIRE(std::is_copy_constructible<decltype(e)>::value);
        REQUIRE(std::is_move_constructible<decltype(e)>::value);
        REQUIRE(std::is_copy_assignable<decltype(e)>::value);
        REQUIRE(std::is_move_assignable<decltype(e)>::value);
        REQUIRE(!TL_EXPECTED_IS_TRIVIALLY_COPY_CONSTRUCTIBLE(decltype(e))::value);
        REQUIRE(!TL_EXPECTED_IS_TRIVIALLY_COPY_ASSIGNABLE(decltype(e))::value);
#if !defined(TL_EXPECTED_GCC49)
        REQUIRE(!std::is_trivially_move_constructible<decltype(e)>::value);
        REQUIRE(!std::is_trivially_move_assignable<decltype(e)>::value);
#endif
    }

    {
        pfs::expected<void,int> e;
        REQUIRE(e);
    }

    {
        pfs::expected<void,int> e (pfs::unexpect, 42);
        REQUIRE(!e);
        REQUIRE(e.error() == 42);
    }
}

TEST_CASE("Assignment") {
    pfs::expected<int, int> e1 = 42;
    pfs::expected<int, int> e2 = 17;
    pfs::expected<int, int> e3 = 21;
    pfs::expected<int, int> e4 = pfs::make_unexpected(42);
    pfs::expected<int, int> e5 = pfs::make_unexpected(17);
    pfs::expected<int, int> e6 = pfs::make_unexpected(21);

    e1 = e2;
    REQUIRE(e1);
    REQUIRE(*e1 == 17);
    REQUIRE(e2);
    REQUIRE(*e2 == 17);

    e1 = std::move(e2);
    REQUIRE(e1);
    REQUIRE(*e1 == 17);
    REQUIRE(e2);
    REQUIRE(*e2 == 17);

    e1 = 42;
    REQUIRE(e1);
    REQUIRE(*e1 == 42);

    auto unex = pfs::make_unexpected(12);
    e1 = unex;
    REQUIRE(!e1);
    REQUIRE(e1.error() == 12);

    e1 = pfs::make_unexpected(42);
    REQUIRE(!e1);
    REQUIRE(e1.error() == 42);

    e1 = e3;
    REQUIRE(e1);
    REQUIRE(*e1 == 21);

    e4 = e5;
    REQUIRE(!e4);
    REQUIRE(e4.error() == 17);

    e4 = std::move(e6);
    REQUIRE(!e4);
    REQUIRE(e4.error() == 21);

    e4 = e1;
    REQUIRE(e4);
    REQUIRE(*e4 == 21);
}

TEST_CASE("Assignment deletion") {
    struct has_all {
        has_all() = default;
        has_all(const has_all &) = default;
        has_all(has_all &&) noexcept = default;
        has_all &operator=(const has_all &) = default;
    };

    pfs::expected<has_all, has_all> e1 = {};
    pfs::expected<has_all, has_all> e2 = {};
    e1 = e2;

    struct except_move {
        except_move() = default;
        except_move(const except_move &) = default;
        except_move(except_move &&) noexcept(false){};
        except_move &operator=(const except_move &) = default;
    };

    // pfs::expected<except_move, except_move> e3 = {};
    // pfs::expected<except_move, except_move> e4 = {};
    // e3 = e4; should not compile
}

TEST_CASE("Emplace") {
    {
        pfs::expected<std::unique_ptr<int>,int> e;
        e.emplace(new int{42});
        REQUIRE(e);
        REQUIRE(**e == 42);
    }

    {
        pfs::expected<std::vector<int>,int> e;
        e.emplace({0,1});
        REQUIRE(e);
        REQUIRE((*e)[0] == 0);
        REQUIRE((*e)[1] == 1);
    }

    {
        pfs::expected<std::tuple<int,int>,int> e;
        e.emplace(2,3);
        REQUIRE(e);
        REQUIRE(std::get<0>(*e) == 2);
        REQUIRE(std::get<1>(*e) == 3);
    }

    {
        pfs::expected<takes_init_and_variadic,int> e = pfs::make_unexpected(0);
        e.emplace({0,1}, 2, 3);
        REQUIRE(e);
        REQUIRE(e->v[0] == 0);
        REQUIRE(e->v[1] == 1);
        REQUIRE(std::get<0>(e->t) == 2);
        REQUIRE(std::get<1>(e->t) == 3);
    }
}

#define TOKENPASTE(x, y) x##y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define STATIC_REQUIRE(e)                                                      \
    constexpr bool TOKENPASTE2(rqure, __LINE__) = e;                           \
    (void)TOKENPASTE2(rqure, __LINE__);                                        \
    REQUIRE(e);

TEST_CASE("Map extensions") {
    auto mul2 = [](int a) {
        return a * 2;
    };

    auto ret_void = [](int /*a*/) {};

    {
        pfs::expected<int, int> e = 21;
        auto ret = e.map(mul2);
        REQUIRE(ret);
        REQUIRE(*ret == 42);
    }

    {
        const pfs::expected<int, int> e = 21;
        auto ret = e.map(mul2);
        REQUIRE(ret);
        REQUIRE(*ret == 42);
    }

    {
        pfs::expected<int, int> e = 21;
        auto ret = std::move(e).map(mul2);
        REQUIRE(ret);
        REQUIRE(*ret == 42);
    }

    {
        const pfs::expected<int, int> e = 21;
        auto ret = std::move(e).map(mul2);
        REQUIRE(ret);
        REQUIRE(*ret == 42);
    }

    {
        pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = e.map(mul2);
        REQUIRE(!ret);
        REQUIRE(ret.error() == 21);
    }

    {
        const pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = e.map(mul2);
        REQUIRE(!ret);
        REQUIRE(ret.error() == 21);
    }

    {
        pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = std::move(e).map(mul2);
        REQUIRE(!ret);
        REQUIRE(ret.error() == 21);
    }

    {
        const pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = std::move(e).map(mul2);
        REQUIRE(!ret);
        REQUIRE(ret.error() == 21);
    }

    {
        pfs::expected<int, int> e = 21;
        auto ret = e.map(ret_void);
        REQUIRE(ret);
        STATIC_REQUIRE(
            (std::is_same<decltype(ret), pfs::expected<void, int>>::value));
    }

    {
        const pfs::expected<int, int> e = 21;
        auto ret = e.map(ret_void);
        REQUIRE(ret);
        STATIC_REQUIRE(
            (std::is_same<decltype(ret), pfs::expected<void, int>>::value));
    }

    {
        pfs::expected<int, int> e = 21;
        auto ret = std::move(e).map(ret_void);
        REQUIRE(ret);
        STATIC_REQUIRE(
            (std::is_same<decltype(ret), pfs::expected<void, int>>::value));
    }

    {
        const pfs::expected<int, int> e = 21;
        auto ret = std::move(e).map(ret_void);
        REQUIRE(ret);
        STATIC_REQUIRE(
            (std::is_same<decltype(ret), pfs::expected<void, int>>::value));
    }

    {
        pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = e.map(ret_void);
        REQUIRE(!ret);
        STATIC_REQUIRE(
            (std::is_same<decltype(ret), pfs::expected<void, int>>::value));
    }

    {
        const pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = e.map(ret_void);
        REQUIRE(!ret);
        STATIC_REQUIRE(
            (std::is_same<decltype(ret), pfs::expected<void, int>>::value));
    }

    {
        pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = std::move(e).map(ret_void);
        REQUIRE(!ret);
        STATIC_REQUIRE(
            (std::is_same<decltype(ret), pfs::expected<void, int>>::value));
    }

    {
        const pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = std::move(e).map(ret_void);
        REQUIRE(!ret);
        STATIC_REQUIRE(
            (std::is_same<decltype(ret), pfs::expected<void, int>>::value));
    }


    // mapping functions which return references
    {
        pfs::expected<int, int> e(42);
        auto ret = e.map([](int& i) -> int& { return i; });
        REQUIRE(ret);
        REQUIRE(ret == 42);
    }
}

TEST_CASE("Map error extensions") {
    auto mul2 = [](int a) {
        return a * 2;
    };
    auto ret_void = [](int /*a*/) {};

    {
        pfs::expected<int, int> e = 21;
        auto ret = e.map_error(mul2);
        REQUIRE(ret);
        REQUIRE(*ret == 21);
    }

    {
        const pfs::expected<int, int> e = 21;
        auto ret = e.map_error(mul2);
        REQUIRE(ret);
        REQUIRE(*ret == 21);
    }

    {
        pfs::expected<int, int> e = 21;
        auto ret = std::move(e).map_error(mul2);
        REQUIRE(ret);
        REQUIRE(*ret == 21);
    }

    {
        const pfs::expected<int, int> e = 21;
        auto ret = std::move(e).map_error(mul2);
        REQUIRE(ret);
        REQUIRE(*ret == 21);
    }

    {
        pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = e.map_error(mul2);
        REQUIRE(!ret);
        REQUIRE(ret.error() == 42);
    }

    {
        const pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = e.map_error(mul2);
        REQUIRE(!ret);
        REQUIRE(ret.error() == 42);
    }

    {
        pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = std::move(e).map_error(mul2);
        REQUIRE(!ret);
        REQUIRE(ret.error() == 42);
    }

    {
        const pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = std::move(e).map_error(mul2);
        REQUIRE(!ret);
        REQUIRE(ret.error() == 42);
    }

    {
        pfs::expected<int, int> e = 21;
        auto ret = e.map_error(ret_void);
        REQUIRE(ret);
    }

    {
        const pfs::expected<int, int> e = 21;
        auto ret = e.map_error(ret_void);
        REQUIRE(ret);
    }

    {
        pfs::expected<int, int> e = 21;
        auto ret = std::move(e).map_error(ret_void);
        REQUIRE(ret);
    }

    {
        const pfs::expected<int, int> e = 21;
        auto ret = std::move(e).map_error(ret_void);
        REQUIRE(ret);
    }

    {
        pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = e.map_error(ret_void);
        REQUIRE(!ret);
    }

    {
        const pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = e.map_error(ret_void);
        REQUIRE(!ret);
    }

    {
        pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = std::move(e).map_error(ret_void);
        REQUIRE(!ret);
    }

    {
        const pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = std::move(e).map_error(ret_void);
        REQUIRE(!ret);
    }
}

TEST_CASE("And then extensions") {
    auto succeed = [](int /*a*/) {
        return pfs::expected<int, int>(21 * 2);
    };
    auto fail = [](int /*a*/) {
        return pfs::expected<int, int>(pfs::unexpect, 17);
    };

    {
        pfs::expected<int, int> e = 21;
        auto ret = e.and_then(succeed);
        REQUIRE(ret);
        REQUIRE(*ret == 42);
    }

    {
        const pfs::expected<int, int> e = 21;
        auto ret = e.and_then(succeed);
        REQUIRE(ret);
        REQUIRE(*ret == 42);
    }

    {
        pfs::expected<int, int> e = 21;
        auto ret = std::move(e).and_then(succeed);
        REQUIRE(ret);
        REQUIRE(*ret == 42);
    }

    {
        const pfs::expected<int, int> e = 21;
        auto ret = std::move(e).and_then(succeed);
        REQUIRE(ret);
        REQUIRE(*ret == 42);
    }

    {
        pfs::expected<int, int> e = 21;
        auto ret = e.and_then(fail);
        REQUIRE(!ret);
        REQUIRE(ret.error() == 17);
    }

    {
        const pfs::expected<int, int> e = 21;
        auto ret = e.and_then(fail);
        REQUIRE(!ret);
        REQUIRE(ret.error() == 17);
    }

    {
        pfs::expected<int, int> e = 21;
        auto ret = std::move(e).and_then(fail);
        REQUIRE(!ret);
        REQUIRE(ret.error() == 17);
    }

    {
        const pfs::expected<int, int> e = 21;
        auto ret = std::move(e).and_then(fail);
        REQUIRE(!ret);
        REQUIRE(ret.error() == 17);
    }

    {
        pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = e.and_then(succeed);
        REQUIRE(!ret);
        REQUIRE(ret.error() == 21);
    }

    {
        const pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = e.and_then(succeed);
        REQUIRE(!ret);
        REQUIRE(ret.error() == 21);
    }

    {
        pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = std::move(e).and_then(succeed);
        REQUIRE(!ret);
        REQUIRE(ret.error() == 21);
    }

    {
        const pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = std::move(e).and_then(succeed);
        REQUIRE(!ret);
        REQUIRE(ret.error() == 21);
    }

    {
        pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = e.and_then(fail);
        REQUIRE(!ret);
        REQUIRE(ret.error() == 21);
    }

    {
        const pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = e.and_then(fail);
        REQUIRE(!ret);
        REQUIRE(ret.error() == 21);
    }

    {
        pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = std::move(e).and_then(fail);
        REQUIRE(!ret);
        REQUIRE(ret.error() == 21);
    }

    {
        const pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = std::move(e).and_then(fail);
        REQUIRE(!ret);
        REQUIRE(ret.error() == 21);
    }
}

TEST_CASE("or_else") {
    using eptr = std::unique_ptr<int>;
    auto succeed = [](int /*a*/) {
        return pfs::expected<int, int>(21 * 2);
    };
    auto succeedptr = [](eptr /*e*/) {
        return pfs::expected<int,eptr>(21*2);
    };
    auto fail =    [](int /*a*/) {
        return pfs::expected<int,int>(pfs::unexpect, 17);
    };
    auto efail =   [](eptr e) {
        *e = 17;
        return pfs::expected<int,eptr>(pfs::unexpect, std::move(e));
    };
    auto failptr = [](eptr e) {
        return pfs::expected<int,eptr>(pfs::unexpect, std::move(e));
    };

    (void)failptr;

    auto failvoid = [](int) {};
    auto failvoidptr = [](const eptr&) { /* don't consume */};
    auto consumeptr = [](eptr) {};
    auto make_u_int = [](int n) {
        return std::unique_ptr<int>(new int(n));
    };

    {
        pfs::expected<int, int> e = 21;
        auto ret = e.or_else(succeed);
        REQUIRE(ret);
        REQUIRE(*ret == 21);
    }

    {
        const pfs::expected<int, int> e = 21;
        auto ret = e.or_else(succeed);
        REQUIRE(ret);
        REQUIRE(*ret == 21);
    }

    {
        pfs::expected<int, int> e = 21;
        auto ret = std::move(e).or_else(succeed);
        REQUIRE(ret);
        REQUIRE(*ret == 21);
    }

    {
        pfs::expected<int, eptr> e = 21;
        auto ret = std::move(e).or_else(succeedptr);
        REQUIRE(ret);
        REQUIRE(*ret == 21);
    }

    {
        const pfs::expected<int, int> e = 21;
        auto ret = std::move(e).or_else(succeed);
        REQUIRE(ret);
        REQUIRE(*ret == 21);
    }

    {
        pfs::expected<int, int> e = 21;
        auto ret = e.or_else(fail);
        REQUIRE(ret);
        REQUIRE(*ret == 21);
    }

    {
        const pfs::expected<int, int> e = 21;
        auto ret = e.or_else(fail);
        REQUIRE(ret);
        REQUIRE(*ret == 21);
    }

    {
        pfs::expected<int, int> e = 21;
        auto ret = std::move(e).or_else(fail);
        REQUIRE(ret);
        REQUIRE(ret == 21);
    }


    {
        pfs::expected<int, eptr> e = 21;
        auto ret = std::move(e).or_else(efail);
        REQUIRE(ret);
        REQUIRE(ret == 21);
    }

    {
        const pfs::expected<int, int> e = 21;
        auto ret = std::move(e).or_else(fail);
        REQUIRE(ret);
        REQUIRE(*ret == 21);
    }

    {
        pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = e.or_else(succeed);
        REQUIRE(ret);
        REQUIRE(*ret == 42);
    }

    {
        const pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = e.or_else(succeed);
        REQUIRE(ret);
        REQUIRE(*ret == 42);
    }

    {
        pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = std::move(e).or_else(succeed);
        REQUIRE(ret);
        REQUIRE(*ret == 42);
    }

    {
        pfs::expected<int, eptr> e(pfs::unexpect, make_u_int(21));
        auto ret = std::move(e).or_else(succeedptr);
        REQUIRE(ret);
        REQUIRE(*ret == 42);
    }

    {
        const pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = std::move(e).or_else(succeed);
        REQUIRE(ret);
        REQUIRE(*ret == 42);
    }

    {
        pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = e.or_else(fail);
        REQUIRE(!ret);
        REQUIRE(ret.error() == 17);
    }

    {
        pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = e.or_else(failvoid);
        REQUIRE(!ret);
        REQUIRE(ret.error() == 21);
    }

    {
        const pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = e.or_else(fail);
        REQUIRE(!ret);
        REQUIRE(ret.error() == 17);
    }

    {
        const pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = e.or_else(failvoid);
        REQUIRE(!ret);
        REQUIRE(ret.error() == 21);
    }

    {
        pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = std::move(e).or_else(fail);
        REQUIRE(!ret);
        REQUIRE(ret.error() == 17);
    }

    {
        pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = std::move(e).or_else(failvoid);
        REQUIRE(!ret);
        REQUIRE(ret.error() == 21);
    }

    {
        pfs::expected<int, eptr> e(pfs::unexpect, make_u_int(21));
        auto ret = std::move(e).or_else(failvoidptr);
        REQUIRE(!ret);
        REQUIRE(*ret.error() == 21);
    }

    {
        pfs::expected<int, eptr> e(pfs::unexpect, make_u_int(21));
        auto ret = std::move(e).or_else(consumeptr);
        REQUIRE(!ret);
        REQUIRE(ret.error() == nullptr);
    }

    {
        const pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = std::move(e).or_else(fail);
        REQUIRE(!ret);
        REQUIRE(ret.error() == 17);
    }

    {
        const pfs::expected<int, int> e(pfs::unexpect, 21);
        auto ret = std::move(e).or_else(failvoid);
        REQUIRE(!ret);
        REQUIRE(ret.error() == 21);
    }
}

struct move_detector {
  move_detector() = default;
  move_detector(move_detector &&rhs) { rhs.been_moved = true; }
  bool been_moved = false;
};

TEST_CASE("Observers") {
    pfs::expected<int,int> o1 = 42;
    pfs::expected<int,int> o2 {pfs::unexpect, 0};
    const pfs::expected<int,int> o3 = 42;

    REQUIRE(*o1 == 42);
    REQUIRE(*o1 == o1.value());
    REQUIRE(o2.value_or(42) == 42);
    REQUIRE(o2.error() == 0);
    REQUIRE(o3.value() == 42);
    auto success = std::is_same<decltype(o1.value()), int &>::value;
    REQUIRE(success);
    success = std::is_same<decltype(o3.value()), const int &>::value;
    REQUIRE(success);
    success = std::is_same<decltype(std::move(o1).value()), int &&>::value;
    REQUIRE(success);

#ifndef TL_EXPECTED_NO_CONSTRR
    success = std::is_same<decltype(std::move(o3).value()), const int &&>::value;
    REQUIRE(success);
#endif

    pfs::expected<move_detector,int> o4{pfs::in_place};
    move_detector o5 = std::move(o4).value();
    REQUIRE(o4->been_moved);
    REQUIRE(!o5.been_moved);
}

struct no_throw {
    no_throw(std::string x) : i(x) {}
    std::string i;
};

struct canthrow_move {
    canthrow_move(std::string x) : i(x) {}
    canthrow_move(canthrow_move const &) = default;
    canthrow_move(canthrow_move &&other) noexcept(false) : i(other.i) {}
    canthrow_move &operator=(canthrow_move &&) = default;
    std::string i;
};

bool should_throw = false;
struct willthrow_move {
    willthrow_move(std::string x) : i(x) {}
    willthrow_move(willthrow_move const &) = default;
    willthrow_move(willthrow_move &&other) : i(other.i) {
        if (should_throw)
            throw 0;
    }
    willthrow_move &operator=(willthrow_move &&) = default;
    std::string i;
};

static_assert(pfs::detail::is_swappable<no_throw>::value, "");

template <class T1, class T2> void swap_test() {
    std::string s1 = "abcdefghijklmnopqrstuvwxyz";
    std::string s2 = "zyxwvutsrqponmlkjihgfedcba";

    pfs::expected<T1, T2> a{s1};
    pfs::expected<T1, T2> b{s2};
    swap(a, b);
    REQUIRE(a->i == s2);
    REQUIRE(b->i == s1);

    a = s1;
    b = pfs::unexpected<T2>(s2);
    swap(a, b);
    REQUIRE(a.error().i == s2);
    REQUIRE(b->i == s1);

    a = pfs::unexpected<T2>(s1);
    b = s2;
    swap(a, b);
    REQUIRE(a->i == s2);
    REQUIRE(b.error().i == s1);

    a = pfs::unexpected<T2>(s1);
    b = pfs::unexpected<T2>(s2);
    swap(a, b);
    REQUIRE(a.error().i == s2);
    REQUIRE(b.error().i == s1);

    a = s1;
    b = s2;
    a.swap(b);
    REQUIRE(a->i == s2);
    REQUIRE(b->i == s1);

    a = s1;
    b = pfs::unexpected<T2>(s2);
    a.swap(b);
    REQUIRE(a.error().i == s2);
    REQUIRE(b->i == s1);

    a = pfs::unexpected<T2>(s1);
    b = s2;
    a.swap(b);
    REQUIRE(a->i == s2);
    REQUIRE(b.error().i == s1);

    a = pfs::unexpected<T2>(s1);
    b = pfs::unexpected<T2>(s2);
    a.swap(b);
    REQUIRE(a.error().i == s2);
    REQUIRE(b.error().i == s1);
}

TEST_CASE("swap") {
    swap_test<no_throw, no_throw>();
    swap_test<no_throw, canthrow_move>();
    swap_test<canthrow_move, no_throw>();

    std::string s1 = "abcdefghijklmnopqrstuvwxyz";
    std::string s2 = "zyxwvutsrqponmlkjihgfedcbaxxx";
    pfs::expected<no_throw, willthrow_move> a{s1};
    pfs::expected<no_throw, willthrow_move> b{pfs::unexpect, s2};
    should_throw = 1;

#ifdef _MSC_VER
    //this seems to break catch on GCC and Clang
    REQUIRE_THROWS(swap(a, b));
#endif

    REQUIRE(a->i == s1);
    REQUIRE(b.error().i == s2);
}
