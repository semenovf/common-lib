////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019,2020 Vladislav Trifochkin
//
// This file is part of [pfs-common](https://github.com/semenovf/pfs-common) library.
//
// Changelog:
//      2019.12.17 Initial version (inspired from https://github.com/mpark/variant)
//      2020.06.12 Added tests inspired from https://github.com/tcbrindle/cpp17_headers)
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/variant.hpp"
#include <memory>
#include <string>
#include <utility>

template <typename ... Types>
using variant = std::variant<Types...>;

////////////////////////////////////////////////////////////////////////////////
//                             Constructors                                   //
////////////////////////////////////////////////////////////////////////////////
TEST_CASE("Variant Constructors")
{
    // Default constructor
    {
        std::variant<int, std::string> v;
        CHECK(0 == std::get<0>(v));

        constexpr std::variant<int> cv;
        static_assert(0 == std::get<0>(cv), "");
    }

    // Copy constructor
    {
        // `v`
        std::variant<int, std::string> v("hello");
        CHECK("hello" == std::get<std::string>(v));

        // `w`
        std::variant<int, std::string> w(v);
        CHECK("hello" == std::get<std::string>(w));

        // Check `v`
        CHECK("hello" == std::get<std::string>(v));

        // `cv`
        constexpr std::variant<int, const char *> cv(42);
        static_assert(42 == std::get<int>(cv), "");

        // `cw`
        // C++17: constexpr is not usable as variant's copy constructor is defaulted.
        // But in MPARK and STX implementations it is not defaulted.
        /*constexpr*/ std::variant<int, const char *> cw(cv);
        /*static_assert*/CHECK(42 == std::get<int>(cw));
    }

    // Forwarding constructors
    // Direct
    {
        std::variant<int, std::string> v(42);
        CHECK(42 == std::get<int>(v));

        constexpr std::variant<int, const char *> cv(42);
        static_assert(42 == std::get<int>(cv), "");
    }

    // DirectConversion
    {
        std::variant<int, std::string> v("42");
        CHECK("42" == std::get<std::string>(v));

        constexpr std::variant<int, const char *> cv(1.1);
        static_assert(1 == std::get<int>(cv), "");
    }

    // CopyInitialization
    {
        std::variant<int, std::string> v = 42;
        CHECK(42 == std::get<int>(v));

        constexpr std::variant<int, const char *> cv = 42;
        static_assert(42 == std::get<int>(cv), "");
    }

    // CopyInitializationConversion
    {
        std::variant<int, std::string> v = "42";
        CHECK("42" == std::get<std::string>(v));

        constexpr std::variant<int, const char *> cv = 1.1;
        static_assert(1 == std::get<int>(cv), "");
    }

    // In-place constructors

    // IndexDirect
    {
        std::variant<int, std::string> v(std::in_place_index_t<0>(), 42);
        CHECK(42 == std::get<0>(v));

        constexpr std::variant<int, const char *> cv(std::in_place_index_t<0>{},42);
        static_assert(42 == std::get<0>(cv), "");
    }

    // IndexDirectDuplicate
    {
        std::variant<int, int> v(std::in_place_index_t<0>(), 42);
        CHECK(42 == std::get<0>(v));

        constexpr std::variant<int, int> cv(std::in_place_index_t<0>{}, 42);
        static_assert(42 == std::get<0>(cv), "");
    }

    // IndexConversion
    {
        std::variant<int, std::string> v(std::in_place_index_t<1>{}, "42");
        CHECK("42" == std::get<1>(v));

        constexpr std::variant<int, const char *> cv(std::in_place_index_t<0>{}, 1.1);
        static_assert(1 == std::get<0>(cv), "");
    }

    // IndexInitializerList
    {
    // FIXME
//         std::variant<int, std::string> v(std::in_place_index_t<1>{}, {'4', '2'});
//         CHECK("42" == std::get<1>(v));
    }

    // TypeDirect
    {
        std::variant<int, std::string> v(std::in_place_type_t<std::string>(), "42");
        CHECK("42" == std::get<std::string>(v));

        constexpr std::variant<int, const char *> cv(
        std::in_place_type_t<int>{}, 42);
        static_assert(42 == std::get<int>(cv), "");
    }

    // TypeConversion
    {
        std::variant<int, std::string> v(std::in_place_type_t<int>(), 42.5);
        CHECK(42 == std::get<int>(v));

        constexpr std::variant<int, const char *> cv(
        std::in_place_type_t<int>{}, 42.5);
        static_assert(42 == std::get<int>(cv), "");
    }

    // TypeInitializerList
    {
        // FIXME
//         std::variant<int, std::string> v(std::in_place_type_t<std::string>(), {'4', '2'});
//         CHECK("42" == std::get<std::string>(v));
    }

    // Move Constructor
    {
        // `v`
        std::variant<int, std::string> v("hello");
        CHECK("hello" == std::get<std::string>(v));
        // `w`
        std::variant<int, std::string> w(std::move(v));
        CHECK("hello" == std::get<std::string>(w));

        // Check `v`
        // FIXME Throws exception "Bad variant index in get"
//         CHECK(std::get<std::string>(v).empty());

        // `cv`
        constexpr std::variant<int, const char *> cv(42);
        static_assert(42 == std::get<int>(cv), "");

        // `cw`
        // FIXME
//         /*constexpr*/ std::variant<int, const char *> cw(std::move(cv));
//         /*static_assert*/CHECK(42 == std::get<int>(cw));
    }
}

////////////////////////////////////////////////////////////////////////////////
//                             Assignments                                    //
////////////////////////////////////////////////////////////////////////////////
TEST_CASE("Variant Assignments") {
    // Same types
    {
        struct Obj {
            Obj () {}
            Obj (Obj const &) {
                FAIL_CHECK("Unexpected call");
            }

            Obj (Obj &&) = default;

            Obj & operator = (Obj const &) {
                // Applied copy assignment operator
                return *this;
            }

            Obj & operator = (Obj &&) = delete;
        };

        // `v`, `w`.
        std::variant<Obj, int> v, w;
        // copy assignment.
        v = w;
    }

    // Different types
    {
        struct Obj {
            Obj () {}
            Obj (Obj const &) {
                // Applied copy constructor
            }

            Obj (Obj &&) = default;

            Obj & operator = (Obj const &) noexcept {
                FAIL_CHECK("Unexpected call");
                return *this;
            }

            Obj & operator = (Obj &&) = delete;
        };

        // `v`, `w`.
        std::variant<Obj, int> v(42), w;
        // copy assignment.
        v = w;
    }

    //
    // Forwarding
    //

    // SameType
    {
        std::variant<int, std::string> v(101);
        CHECK(101 == std::get<int>(v));
        v = 202;
        CHECK(202 == std::get<int>(v));
    }

    // SameTypeFwd
    {
        std::variant<int, std::string> v(1.1);
        CHECK(1 == std::get<int>(v));
        v = 2.2;
        CHECK(2 == std::get<int>(v));
    }

    // DiffType
    {
        std::variant<int, std::string> v(42);
        CHECK(42 == std::get<int>(v));
        v = "42";
        CHECK("42" == std::get<std::string>(v));
    }

    // DiffTypeFwd
    {
        std::variant<int, std::string> v(42);
        CHECK(42 == std::get<int>(v));
        v = "42";
        CHECK("42" == std::get<std::string>(v));
    }

    // ExactMatch
    {
        std::variant<const char *, std::string> v;
        v = std::string("hello");
        CHECK("hello" == std::get<std::string>(v));
    }

    // BetterMatch
    {
        std::variant<int, double> v;
        // `char` -> `int` is better than `char` -> `double`

        // FIXME
//         v = 'x';
//         CHECK(static_cast<int>('x') == std::get<int>(v));
    }

    // NoMatch
    {
        struct x {};
        // FIXME
//         static_assert(!std::is_assignable<std::variant<int, std::string>, x> {},
//                       "variant<int, std::string> v; v = x;");
    }

    // Ambiguous
    {
        // FIXME
//         static_assert(!std::is_assignable<std::variant<short, long>, int> {},
//             "variant<short, long> v; v = 42;");
    }

    // SameTypeOptimization
    {
        std::variant<int, std::string> v("hello world!");
        // Check `v`.
        std::string const & x = std::get<std::string>(v);
        CHECK("hello world!" == x);

        // Save the "hello world!"'s capacity.
        int capacity = x.capacity();

        // Use `std::string::operator=(const char *)` to assign into `v`.
        v = "hello";

        // Check `v`.
        std::string const & y = std::get<std::string>(v);

        CHECK("hello" == y);

        // Since "hello" is shorter than "hello world!", we should have preserved the
        // existing capacity of the string!.
        CHECK(capacity == y.capacity());
    }

    //
    // Movable assignments
    //
    // SameType
    {
        struct Obj {
            constexpr Obj () {}
            Obj (Obj const &) = delete;
            Obj (Obj &&) noexcept {
                FAIL_CHECK("Unexpected call");
            }

            Obj & operator = (Obj const &) = delete;
            Obj & operator = (Obj &&) noexcept {
                // Applied movable assignment operator
                return *this;
            }
        };

        // `v`, `w`.
        std::variant<Obj, int> v, w;
        // move assignment.
        v = std::move(w);
    }

    // DiffType
    {
        struct Obj {
            constexpr Obj () {}
            Obj (Obj const &) = delete;
            Obj (Obj &&) noexcept {
                // Applied movable constructor
            }
            Obj & operator = (Obj const &) = delete;
            Obj & operator = (Obj &&) noexcept {
                FAIL_CHECK("Unexpected call");
                return *this;
            }
        };
        // `v`, `w`.
        std::variant<Obj, int> v(42), w;
        // move assignment.
        v = std::move(w);
    }
}

////////////////////////////////////////////////////////////////////////////////
//                             Destructors                                    //
////////////////////////////////////////////////////////////////////////////////
TEST_CASE("Variant Destructor") {
    struct Obj {
        Obj(bool & dtor_called) : dtor_called_(dtor_called) {}

        ~Obj() {
            dtor_called_ = true;
        }
        bool & dtor_called_;
    };  // Obj

    bool dtor_called = false;

    {
        std::variant<Obj> v(std::in_place_type_t<Obj>(), dtor_called);
    }

    CHECK(dtor_called);
}

////////////////////////////////////////////////////////////////////////////////
//                                  Get                                       //
////////////////////////////////////////////////////////////////////////////////
TEST_CASE("Variant Get") {

    // HoldsAlternative
    {
        std::variant<int, std::string> v(42);
//         CHECK(std::holds_alternative<0>(v)); // FIXME or remove
//         CHECK(!std::holds_alternative<1>(v)); // FIXME or remove
        CHECK(std::holds_alternative<int>(v));
        CHECK(!std::holds_alternative<std::string>(v));

        constexpr std::variant<int, const char *> cv(42);
//         static_assert(std::holds_alternative<0>(cv), ""); // FIXME or remove
//         static_assert(!std::holds_alternative<1>(cv), ""); // FIXME or remove
        static_assert(std::holds_alternative<int>(cv), "");
        static_assert(!std::holds_alternative<const char *>(cv), "");
    }

    // MutVarMutType
    {
        std::variant<int> v(42);
        CHECK(42 == std::get<int>(v));
    }

    // MutVarConstType
    {
        // FIXME
//         std::variant<int const> v(42);
//         CHECK(42 == std::get<int const>(v));
    }

    // ConstVarMutType
    {
        std::variant<int> const v(42);
        CHECK(42 == std::get<int>(v));

        constexpr std::variant<int> cv(42);
        static_assert(42 == std::get<int>(cv), "");
    }

    // ConstVarConstType
    {
        // FIXME
//         const std::variant<const int> v(42);
//         CHECK(42 == std::get<const int>(v));
//
//         constexpr std::variant<const int> cv(42);
//         static_assert(42 == std::get<const int>(cv), "");
    }

    // MutVarMutType
    {
        std::variant<int> v(42);
        auto n = std::get_if<int>(& v);
        CHECK(42 == *std::get_if<int>(& v));
    }

    // MutVarConstType
    {
        // FIXME
//         std::variant<const int> v(42);
//         CHECK(42 == *std::get_if<const int>(& v));
    }

    // ConstVarMutType
    {
        const std::variant<int> v(42);
        CHECK(42 == *std::get_if<int>(&v));

        static constexpr std::variant<int> cv(42);
        static_assert(42 == *std::get_if<int>(& cv), "");
    }

    // ConstVarConstType
    {
        // FIXME
//         const std::variant<const int> v(42);
//         CHECK(42 == *std::get_if<const int>(& v));

        // FIXME
//         static constexpr std::variant<const int> cv(42);
//         static_assert(42 == *std::get_if<const int>(& cv), "");
    }
}

// ////////////////////////////////////////////////////////////////////////////////
// //                                 Modifiers                                  //
// ////////////////////////////////////////////////////////////////////////////////
TEST_CASE("Variant Modifiers") {

    // IndexDirect
    {
        std::variant<int, std::string> v;
        v.emplace<1>("42");
        CHECK("42" == std::get<1>(v));
    }

    // IndexDirectDuplicate
    {
        std::variant<int, int> v;
        v.emplace<1>(42);
        CHECK(42 == std::get<1>(v));
    }

    // IndexConversion
    {
        std::variant<int, std::string> v;
        v.emplace<1>("42");
        CHECK("42" == std::get<1>(v));
    }

    // IndexConversionDuplicate
    {
        std::variant<int, int> v;
        v.emplace<1>(1.1);
        CHECK(1 == std::get<1>(v));
    }

    // IndexInitializerList
    {
        std::variant<int, std::string> v;

        // FIXME
//         v.emplace<1>({'4', '2'});
//         CHECK("42" == std::get<1>(v));
    }

    // TypeDirect
    {
        std::variant<int, std::string> v;
        v.emplace<std::string>("42");
        CHECK("42" == std::get<std::string>(v));
    }

    // TypeConversion
    {
        std::variant<int, std::string> v;
        v.emplace<int>(1.1);
        CHECK(1 == std::get<int>(v));
    }

    // TypeInitializerList
    {
        std::variant<int, std::string> v;
        // FIXME
//         v.emplace<std::string>({'4', '2'});
//         CHECK("42" == std::get<std::string>(v));
    }
}

////////////////////////////////////////////////////////////////////////////////
//                                 Comparisons                                //
////////////////////////////////////////////////////////////////////////////////
TEST_CASE("Variant Comparisons") {

    // SameTypeSameValue
    {
        std::variant<int, std::string> v(0), w(0);
        // `v` op `w`
        CHECK(v == w);
        CHECK_FALSE(v != w);
        CHECK_FALSE(v < w);
        CHECK_FALSE(v > w);
        CHECK(v <= w);
        CHECK(v >= w);
        // `w` op `v`
        CHECK(w == v);
        CHECK_FALSE(w != v);
        CHECK_FALSE(w < v);
        CHECK_FALSE(w > v);
        CHECK(w <= v);
        CHECK(w >= v);

#if (!defined(__GNUC__) || __GNUC__ >= 5)
        constexpr std::variant<int, const char *> cv(0), cw(0);
        // `cv` op `cw`
        static_assert(cv == cw, "");   // g++ error (prior to version 5): non-constant condition for static assertion
        static_assert(!(cv != cw), "");
        static_assert(!(cv < cw), "");
        static_assert(!(cv > cw), "");
        static_assert(cv <= cw, "");
        static_assert(cv >= cw, "");
        // `cw` op `cv`
        static_assert(cw == cv, "");
        static_assert(!(cw != cv), "");
        static_assert(!(cw < cv), "");
        static_assert(!(cw > cv), "");
        static_assert(cw <= cv, "");
        static_assert(cw >= cv, "");
#endif
    }

    // SameTypeDiffValue
    {
        std::variant<int, std::string> v(0), w(1);
        // `v` op `w`
        CHECK_FALSE(v == w);
        CHECK(v != w);
        CHECK(v < w);
        CHECK_FALSE(v > w);
        CHECK(v <= w);
        CHECK_FALSE(v >= w);
        // `w` op `v`
        CHECK_FALSE(w == v);
        CHECK(w != v);
        CHECK_FALSE(w < v);
        CHECK(w > v);
        CHECK_FALSE(w <= v);
        CHECK(w >= v);

#if (!defined(__GNUC__) || __GNUC__ >= 5)
        constexpr std::variant<int, const char *> cv(0), cw(1);
        // `cv` op `cw`
        static_assert(!(cv == cw), "");
        static_assert(cv != cw, "");
        static_assert(cv < cw, "");
        static_assert(!(cv > cw), "");
        static_assert(cv <= cw, "");
        static_assert(!(cv >= cw), "");
        // `cw` op `cv`
        static_assert(!(cw == cv), "");
        static_assert(cw != cv, "");
        static_assert(!(cw < cv), "");
        static_assert(cw > cv, "");
        static_assert(!(cw <= cv), "");
        static_assert(cw >= cv, "");
#endif
    }

    // DiffTypeSameValue
    {
        std::variant<int, unsigned int> v(0), w(0u);
        // `v` op `w`
        CHECK_FALSE(v == w);
        CHECK(v != w);
        CHECK(v < w);
        CHECK_FALSE(v > w);
        CHECK(v <= w);
        CHECK_FALSE(v >= w);
        // `w` op `v`
        CHECK_FALSE(w == v);
        CHECK(w != v);
        CHECK_FALSE(w < v);
        CHECK(w > v);
        CHECK_FALSE(w <= v);
        CHECK(w >= v);

#if (!defined(__GNUC__) || __GNUC__ >= 5)
        constexpr std::variant<int, unsigned int> cv(0), cw(0u);
        // `cv` op `cw`
        static_assert(!(cv == cw), "");
        static_assert(cv != cw, "");
        static_assert(cv < cw, "");
        static_assert(!(cv > cw), "");
        static_assert(cv <= cw, "");
        static_assert(!(cv >= cw), "");
        // `cw` op `cv`
        static_assert(!(cw == cv), "");
        static_assert(cw != cv, "");
        static_assert(!(cw < cv), "");
        static_assert(cw > cv, "");
        static_assert(!(cw <= cv), "");
        static_assert(cw >= cv, "");
#endif
    }

    // DiffTypeDiffValue
    {
        std::variant<int, unsigned int> v(0), w(1u);
        // `v` op `w`
        CHECK_FALSE(v == w);
        CHECK(v != w);
        CHECK(v < w);
        CHECK_FALSE(v > w);
        CHECK(v <= w);
        CHECK_FALSE(v >= w);
        // `w` op `v`
        CHECK_FALSE(w == v);
        CHECK(w != v);
        CHECK_FALSE(w < v);
        CHECK(w > v);
        CHECK_FALSE(w <= v);
        CHECK(w >= v);

#if (!defined(__GNUC__) || __GNUC__ >= 5)
        constexpr std::variant<int, unsigned int> cv(0), cw(1u);
        // `cv` op `cw`
        static_assert(!(cv == cw), "");
        static_assert(cv != cw, "");
        static_assert(cv < cw, "");
        static_assert(!(cv > cw), "");
        static_assert(cv <= cw, "");
        static_assert(!(cv >= cw), "");
        // `cw` op `cv`
        static_assert(!(cw == cv), "");
        static_assert(cw != cv, "");
        static_assert(!(cw < cv), "");
        static_assert(cw > cv, "");
        static_assert(!(cw <= cv), "");
        static_assert(cw >= cv, "");
#endif
    }
}

////////////////////////////////////////////////////////////////////////////////
//                                 Swap                                       //
////////////////////////////////////////////////////////////////////////////////
namespace swap1 {

struct Obj
{
    Obj (size_t * dtor_count) : dtor_count_(dtor_count) {}
    ~Obj () { ++(*dtor_count_); }
    size_t * dtor_count_;
};

} // namespace swap1

namespace swap2 {

struct Obj
{
    Obj (size_t * dtor_count) : dtor_count_(dtor_count) {}
    ~Obj () { ++(*dtor_count_); }
    size_t * dtor_count_;
};

static void swap (Obj &lhs, Obj &rhs) noexcept
{
    std::swap(lhs.dtor_count_, rhs.dtor_count_);
}

} // namespace swap2

namespace swap3 {

struct V
{
    V (size_t *dtor_count) : dtor_count_(dtor_count) {}
    ~V () { ++(*dtor_count_); }
    size_t * dtor_count_;
};

struct W
{
    W (size_t *dtor_count) : dtor_count_(dtor_count) {}
    ~W () { ++(*dtor_count_); }
    size_t * dtor_count_;
};

} // namespace swap2

TEST_CASE("Variant Swap") {
    // Same
    {
        std::variant<int, std::string> v("hello");
        std::variant<int, std::string> w("world");
        // Check `v`.
        CHECK("hello" == std::get<std::string>(v));
        // Check `w`.
        CHECK("world" == std::get<std::string>(w));
        // Swap.
        using std::swap;
        swap(v, w);
        // Check `v`.
        CHECK("world" == std::get<std::string>(v));
        // Check `w`.
        CHECK("hello" == std::get<std::string>(w));
    }

    // Different
    {
        std::variant<int, std::string> v(42);
        std::variant<int, std::string> w("hello");
        // Check `v`.
        CHECK(42 == std::get<int>(v));
        // Check `w`.
        CHECK("hello" == std::get<std::string>(w));
        // Swap.
        using std::swap;
        swap(v, w);
        // Check `v`.
        CHECK("hello" == std::get<std::string>(v));
        // Check `w`.
        CHECK(42 == std::get<int>(w));
    }

    // DtorsSame
    {
        size_t v_count = 0;
        size_t w_count = 0;
        {
            std::variant<swap1::Obj> v(&v_count), w(&w_count);
            // Calls `std::swap(Obj &lhs, Obj &rhs)`, with which we perform:
            // ```
            // {
            //   Obj temp(move(lhs));
            //   lhs = move(rhs);
            //   rhs = move(temp);
            // }  `++v_count` from `temp::~Obj()`.
            // ```
            using std::swap;
            swap(v, w);
            CHECK(1u == v_count);
            CHECK(0u == w_count);
        }
        CHECK(2u == v_count);
        CHECK(1u == w_count);
    }

    // DtorsSameWithSwap
    {
        size_t v_count = 0;
        size_t w_count = 0;
        {
            std::variant<swap2::Obj> v(& v_count), w(& w_count);
            using std::swap;
            swap(v, w);
            // Calls `swap2::swap(Obj &lhs, Obj &rhs)`, with which doesn't call any destructors.
            CHECK(0u == v_count);
            CHECK(0u == w_count);
        }
        CHECK(1u == v_count);
        CHECK(1u == w_count);
    }

    // DtorsDifferent
    {
        size_t v_count = 0;
        size_t w_count = 0;
        {
            std::variant<swap3::V, swap3::W> v(std::in_place_type_t<swap3::V>(), & v_count);
            std::variant<swap3::V, swap3::W> w(std::in_place_type_t<swap3::W>(), & w_count);
            using std::swap;
            swap(v, w);
            CHECK(1u == v_count);
            CHECK(2u == w_count);
        }
        CHECK(2u == v_count);
        CHECK(3u == w_count);
    }
}

////////////////////////////////////////////////////////////////////////////////
//                                 Visit                                       //
////////////////////////////////////////////////////////////////////////////////
struct concat
{
    template <typename... Args>
    std::string operator()(const Args &... args) const
    {
        std::ostringstream strm;
        std::initializer_list<int>({(strm << args, 0)...});
        return std::move(strm).str();
    }
};

TEST_CASE("Variant Visit") {

    enum Qual { LRef, ConstLRef, RRef, ConstRRef };

    struct get_qual
    {
        constexpr Qual operator () (int &) const      { return LRef; }
        constexpr Qual operator()(const int &) const  { return ConstLRef; }
        constexpr Qual operator()(int &&) const       { return RRef; }
        constexpr Qual operator()(const int &&) const { return ConstRRef; }
    };

    // MutVarMutType
    {
        std::variant<int> v(42);
        // Check `v`.
        CHECK(42 == std::get<int>(v));

        // FIXME
        // Check qualifier.
//         CHECK(LRef == std::visit(get_qual(), v));
//         CHECK(RRef == std::visit(get_qual(), std::move(v)));
    }

    // MutVarConstType
    {
        // FIXME
//         std::variant<const int> v(42);
//         CHECK(42 == std::get<const int>(v));
        // FIXME
        // Check qualifier.
//         CHECK(ConstLRef == std::visit(get_qual(), v));
//         CHECK(ConstRRef == std::visit(get_qual(), std::move(v)));
    }

    // ConstVarMutType
    {
        const std::variant<int> v(42);
        CHECK(42 == std::get<int>(v));

        // FIXME
        // Check qualifier.
//         CHECK(ConstLRef == std::visit(get_qual(), v));
//         CHECK(ConstRRef == std::visit(get_qual(), std::move(v)));
    }

    // ConstVarConstType
    {
        // FIXME
//         const std::variant<const int> v(42);
//         CHECK(42 == std::get<const int>(v));
//         // Check qualifier.
//         CHECK(ConstLRef == std::visit(get_qual(), v));
//         CHECK(ConstRRef == std::visit(get_qual(), std::move(v)));
    }

    // Zero
//     {
//         CHECK("" == std::visit(concat{}));
//     }

    // Double
    // FIXME
//     {
//         std::variant<int, std::string> v("hello"), w("world!");
//         CHECK("helloworld!" == std::visit(concat {}, v, w));
//     }

// FIXME
//     // Quintuple
//     {
//         std::variant<int, std::string> v(101), w("+"), x(202), y("="), z(303);
//         CHECK("101+202=303" == std::visit(concat {}, v, w, x, y, z));
//     }
//
//     // Double
//     {
//         std::variant<int, std::string> v("hello");
//         std::variant<double, const char *> w("world!");
//         CHECK("helloworld!" == std::visit(concat{}, v, w));
//     }
//
//     // Quintuple)
//     {
//         std::variant<int, double> v(101);
//         std::variant<const char *> w("+");
//         std::variant<bool, std::string, int> x(202);
//         std::variant<char, std::string, const char *> y('=');
//         std::variant<long, short> z(303L);
//         CHECK("101+202=303" == std::visit(concat {}, v, w, x, y, z));
//     }
}

////////////////////////////////////////////////////////////////////////////////

TEST_CASE("initial_is_first_type") {
    variant<int> v;
    CHECK_FALSE(v.valueless_by_exception());
    CHECK(v.index() == 0);
    CHECK(std::get<int>(v) == 0);
}

TEST_CASE("can_construct_first_type") {
    variant<int> v(42);
    CHECK(v.index() == 0);
}

TEST_CASE("can_get_value_of_first_type") {
    variant<int> v(42);
    int & i = std::get<int>(v);
    CHECK(i == 42);
}

TEST_CASE("can_construct_second_type") {
    variant<int,std::string> v(std::string("hello"));
    CHECK(v.index()==1);
    std::string & s = std::get<std::string>(v);
    CHECK(s=="hello");
}

TEST_CASE("can_move_variant") {
    variant<int,std::string> v(std::string("hello"));
    variant<int,std::string> v2(std::move(v));
    CHECK(v2.index() == 1);
    CHECK(v.index() == -1);
    std::string & s = std::get<std::string>(v2);
    CHECK(s == "hello");
}

TEST_CASE("can_copy_variant") {
    variant<int,std::string> v(std::string("hello"));
    variant<int,std::string> v2(v);
    CHECK(v2.index()==1);
    CHECK(v.index()==1);
    std::string& s=std::get<std::string>(v);
    CHECK(s=="hello");
    std::string & s2=std::get<std::string>(v2);
    CHECK(s2=="hello");
    CHECK(&s!=&s2);
}

TEST_CASE("can_copy_const_variant") {
    variant<int,std::string> const v(std::string("hello"));
    variant<int,std::string> v2(v);
    CHECK(v2.index()==1);
    CHECK(v.index()==1);
    std::string const& s=std::get<std::string>(v);
    CHECK(s=="hello");
    std::string& s2=std::get<std::string>(v2);
    CHECK(s2=="hello");
    CHECK(&s!=&s2);
}

TEST_CASE("construction_from_lvalue") {
    std::vector<int> vec(42);
    variant<std::vector<int>> v(vec);
    CHECK(vec.size()==42);
    CHECK(v.index()==0);
    std::vector<int>& vec2=std::get<std::vector<int>>(v);
    CHECK(&vec2!=&vec);
    CHECK(vec2.size()==42);
}

TEST_CASE("construction_from_const_lvalue") {
    std::vector<int> const vec(42);
    variant<std::vector<int>> v(vec);
    CHECK(vec.size()==42);
    CHECK(v.index()==0);
    std::vector<int>& vec2=std::get<std::vector<int>>(v);
    CHECK(&vec2!=&vec);
    CHECK(vec2.size()==42);
}

TEST_CASE("move_construction_with_move_only_types") {
    std::unique_ptr<int> ui(new int(42));
    variant<std::unique_ptr<int>> v(std::move(ui));
    CHECK(v.index()==0);
    std::unique_ptr<int>& p2=std::get<std::unique_ptr<int>>(v);
    CHECK(p2);
    CHECK(*p2==42);

    variant<std::unique_ptr<int>> v2(std::move(v));
    CHECK(v.index()==-1);
    CHECK(v2.index()==0);
    std::unique_ptr<int>& p3=std::get<std::unique_ptr<int>>(v2);
    CHECK(p3);
    CHECK(*p3==42);
}

struct CopyCounter
{
    unsigned move_construct=0;
    unsigned copy_construct=0;
    unsigned move_assign=0;
    unsigned copy_assign=0;

    CopyCounter() noexcept {}

    CopyCounter(const CopyCounter& rhs) noexcept:
        move_construct(rhs.move_construct),
        copy_construct(rhs.copy_construct+1),
        move_assign(rhs.move_assign),
        copy_assign(rhs.copy_assign)
    {}

    CopyCounter(CopyCounter&& rhs) noexcept:
        move_construct(rhs.move_construct+1),
        copy_construct(rhs.copy_construct),
        move_assign(rhs.move_assign),
        copy_assign(rhs.copy_assign)
    {}

    CopyCounter& operator=(const CopyCounter& rhs) noexcept{
        move_construct=rhs.move_construct;
        copy_construct=rhs.copy_construct;
        move_assign=rhs.move_assign;
        copy_assign=rhs.copy_assign+1;
        return *this;
    }

    CopyCounter& operator=(CopyCounter&& rhs) noexcept{
        move_construct=rhs.move_construct;
        copy_construct=rhs.copy_construct;
        move_assign=rhs.move_assign+1;
        copy_assign=rhs.copy_assign;
        return *this;
    }
};

TEST_CASE("copy_assignment_same_type") {
    CopyCounter cc;
    variant<CopyCounter> v(cc);
    CHECK(v.index()==0);
    CHECK(std::get<CopyCounter>(v).copy_construct==1);
    CHECK(std::get<CopyCounter>(v).move_construct==0);
    CHECK(std::get<CopyCounter>(v).copy_assign==0);
    CHECK(std::get<CopyCounter>(v).move_assign==0);

    variant<CopyCounter> v2(cc);
    v2=v;
    CHECK(v2.index()==0);
    CHECK(std::get<CopyCounter>(v2).copy_construct==1);
    CHECK(std::get<CopyCounter>(v2).move_construct==0);
    CHECK(std::get<CopyCounter>(v2).copy_assign==1);
    CHECK(std::get<CopyCounter>(v2).move_assign==0);
}

struct ThrowingConversion{
    template<typename T>
    operator T() const{
        throw 42;
    }
};

template<typename V>
void empty_variant(V& v){
    try{
        v.template emplace<0>(ThrowingConversion());
    }
    catch(int){}
    CHECK(v.valueless_by_exception());
}

TEST_CASE("copy_assignment_to_empty") {
    CopyCounter cc;
    variant<CopyCounter> v(cc);
    CHECK(v.index()==0);
    CHECK(std::get<CopyCounter>(v).copy_construct==1);
    CHECK(std::get<CopyCounter>(v).move_construct==0);
    CHECK(std::get<CopyCounter>(v).copy_assign==0);
    CHECK(std::get<CopyCounter>(v).move_assign==0);

    variant<CopyCounter> v2;
    empty_variant(v2);
    v2=v;
    CHECK(v.index()==0);
    CHECK(v2.index()==0);
    CHECK(std::get<CopyCounter>(v2).copy_construct==2);
    CHECK(std::get<CopyCounter>(v2).move_construct==0);
    CHECK(std::get<CopyCounter>(v2).copy_assign==0);
    CHECK(std::get<CopyCounter>(v2).move_assign==0);
}

struct InstanceCounter{
    static unsigned instances;

    InstanceCounter(){
        ++instances;
    }
    InstanceCounter(InstanceCounter const& rhs){
        ++instances;
    }
    ~InstanceCounter(){
        --instances;
    }
};

unsigned InstanceCounter::instances=0;

TEST_CASE("copy_assignment_of_diff_types_destroys_old") {
    variant<InstanceCounter,int> v;
    CHECK(InstanceCounter::instances==1);
    v=variant<InstanceCounter,int>(InstanceCounter());
    CHECK(v.index()==0);
    CHECK(InstanceCounter::instances==1);
    variant<InstanceCounter,int> v2(42);
    v=v2;
    CHECK(v.index()==1);
    CHECK(v2.index()==1);
    CHECK(std::get<int>(v2)==42);
    CHECK(std::get<int>(v)==42);
    CHECK(InstanceCounter::instances==0);
    v=variant<InstanceCounter,int>(InstanceCounter());
    CHECK(InstanceCounter::instances==1);
    variant<InstanceCounter,int> const v3(42);
    v=v3;
    CHECK(v.index()==1);
    CHECK(v3.index()==1);
    CHECK(std::get<int>(v3)==42);
    CHECK(std::get<int>(v)==42);
    CHECK(InstanceCounter::instances==0);
}

TEST_CASE("copy_assignment_from_empty") {
    variant<InstanceCounter,int> v=InstanceCounter();
    CHECK(v.index()==0);
    CHECK(InstanceCounter::instances==1);
    variant<InstanceCounter,int> v2;
    empty_variant(v2);
    v=v2;
    CHECK(v.index()==-1);
    CHECK(v2.index()==-1);
    CHECK(InstanceCounter::instances==0);
}

struct CopyError {};

struct ThrowingCopy{
    int data;

    ThrowingCopy():data(0){}
    ThrowingCopy(ThrowingCopy const&){
        throw CopyError();
    }
    ThrowingCopy(ThrowingCopy&&){
        throw CopyError();
    }
    ThrowingCopy operator=(ThrowingCopy const&){
        throw CopyError();
    }
};

TEST_CASE("throwing_copy_assign_leaves_target_unchanged") {
    variant<std::string,ThrowingCopy> v = std::string("hello");
    CHECK(v.index()==0);
    variant<std::string,ThrowingCopy> v2{std::in_place_type_t<ThrowingCopy>{}};

    try {
        v = v2;
        CHECK(!"Exception should be thrown");
    } catch (CopyError &) {
        //
    }

    CHECK(v.index()==0);
    CHECK(std::get<0>(v)=="hello");
}

TEST_CASE("move_assignment_to_empty") {
    CopyCounter cc;
    variant<CopyCounter> v(cc);
    CHECK(v.index()==0);
    CHECK(std::get<CopyCounter>(v).copy_construct==1);
    CHECK(std::get<CopyCounter>(v).move_construct==0);
    CHECK(std::get<CopyCounter>(v).copy_assign==0);
    CHECK(std::get<CopyCounter>(v).move_assign==0);

    variant<CopyCounter> v2;
    empty_variant(v2);
    v2=std::move(v);
    CHECK(v.index()==-1);
    CHECK(v2.index()==0);
    CHECK(std::get<CopyCounter>(v2).copy_construct==1);
    CHECK(std::get<CopyCounter>(v2).move_construct==1);
    CHECK(std::get<CopyCounter>(v2).copy_assign==0);
    CHECK(std::get<CopyCounter>(v2).move_assign==0);
}

TEST_CASE("move_assignment_same_type") {
    CopyCounter cc;
    variant<CopyCounter> v(cc);
    CHECK(v.index()==0);
    CHECK(std::get<CopyCounter>(v).copy_construct==1);
    CHECK(std::get<CopyCounter>(v).move_construct==0);
    CHECK(std::get<CopyCounter>(v).copy_assign==0);
    CHECK(std::get<CopyCounter>(v).move_assign==0);

    variant<CopyCounter> v2(std::move(cc));
    v2=std::move(v);
    CHECK(v.index()==-1);
    CHECK(v2.index()==0);
    CHECK(std::get<CopyCounter>(v2).copy_construct==1);
    CHECK(std::get<CopyCounter>(v2).move_construct==0);
    CHECK(std::get<CopyCounter>(v2).copy_assign==0);
    CHECK(std::get<CopyCounter>(v2).move_assign==1);
}

TEST_CASE("move_assignment_of_diff_types_destroys_old") {
    variant<InstanceCounter,CopyCounter> v;
    CHECK(InstanceCounter::instances==1);
    empty_variant(v);
    CHECK(InstanceCounter::instances==0);
    v=variant<InstanceCounter,CopyCounter>(InstanceCounter());
    CHECK(v.index()==0);
    CHECK(InstanceCounter::instances==1);
    variant<InstanceCounter,CopyCounter> v2{CopyCounter()};
    v=std::move(v2);
    CHECK(v.index()==1);
    CHECK(v2.index()==-1);
    CHECK(InstanceCounter::instances==0);
    CHECK(std::get<CopyCounter>(v).copy_construct==0);
    CHECK(std::get<CopyCounter>(v).move_construct==2);
    CHECK(std::get<CopyCounter>(v).copy_assign==0);
    CHECK(std::get<CopyCounter>(v).move_assign==0);
}

TEST_CASE("move_assignment_from_empty") {
    variant<InstanceCounter,int> v=InstanceCounter();
    CHECK(v.index()==0);
    CHECK(InstanceCounter::instances==1);
    variant<InstanceCounter,int> v2;
    empty_variant(v2);
    v = std::move(v2);
    CHECK(v.index()==-1);
    CHECK(v2.index()==-1);
    CHECK(InstanceCounter::instances==0);
}

TEST_CASE("emplace_construction_by_type") {
    const char* const msg = "hello";
    variant<int,char const*,std::string> v(
        std::in_place_type_t<std::string>{}, msg);
    CHECK(v.index()==2);
    CHECK(std::get<2>(v)==msg);
}

TEST_CASE("emplace_construction_by_index") {
    const char* const msg="hello";
    variant<int,char const*,std::string> v(
        std::in_place_index_t<2>{}, msg);
    CHECK(v.index()==2);
    CHECK(std::get<2>(v)==msg);
}

TEST_CASE("holds_alternative_for_empty_variant") {
    variant<int,double> v;
    empty_variant(v);
    CHECK(!std::holds_alternative<int>(v));
    CHECK(!std::holds_alternative<double>(v));
}

TEST_CASE("holds_alternative_for_non_empty_variant") {
    variant<int,double> v(2.3);
    CHECK(!std::holds_alternative<int>(v));
    CHECK(std::holds_alternative<double>(v));
}

TEST_CASE("assignment_from_value_to_empty") {
    CopyCounter cc;
    variant<int,CopyCounter> v;
    v=cc;
    CHECK(v.index()==1);
    CHECK(std::get<CopyCounter>(v).copy_construct==1);
    CHECK(std::get<CopyCounter>(v).move_construct==0);
    CHECK(std::get<CopyCounter>(v).copy_assign==0);
    CHECK(std::get<CopyCounter>(v).move_assign==0);
}

TEST_CASE("assignment_from_value_to_same_type") {
    CopyCounter cc;
    variant<int,CopyCounter> v(cc);
    v=cc;
    CHECK(v.index()==1);
    CHECK(std::get<CopyCounter>(v).copy_construct==0);
    CHECK(std::get<CopyCounter>(v).move_construct==0);
    CHECK(std::get<CopyCounter>(v).copy_assign==1);
    CHECK(std::get<CopyCounter>(v).move_assign==0);
}

TEST_CASE("assignment_from_value_of_diff_types_destroys_old") {
    variant<InstanceCounter,CopyCounter> v{InstanceCounter()};
    CHECK(v.index()==0);
    CHECK(InstanceCounter::instances==1);
    v=CopyCounter();
    CHECK(v.index()==1);
    CHECK(InstanceCounter::instances==0);
    CHECK(std::get<CopyCounter>(v).copy_construct==0);
    CHECK(std::get<CopyCounter>(v).move_construct==1);
    CHECK(std::get<CopyCounter>(v).copy_assign==0);
    CHECK(std::get<CopyCounter>(v).move_assign==0);
}

TEST_CASE("emplace_from_value_to_empty") {
    const char* const msg="hello";
    variant<int,char const*,std::string> v;
    v.emplace<std::string>(msg);
    CHECK(v.index()==2);
    CHECK(std::get<2>(v)==msg);
}

TEST_CASE("emplace_from_value_to_same_type") {
    CopyCounter cc;
    variant<int,CopyCounter> v(cc);
    v.emplace<CopyCounter>();
    CHECK(v.index()==1);
    CHECK(std::get<CopyCounter>(v).copy_construct==0);
    CHECK(std::get<CopyCounter>(v).move_construct==0);
    CHECK(std::get<CopyCounter>(v).copy_assign==0);
    CHECK(std::get<CopyCounter>(v).move_assign==0);
}

TEST_CASE("emplace_from_value_of_diff_types_destroys_old") {
    variant<InstanceCounter,CopyCounter> v{InstanceCounter()};
    CHECK(v.index()==0);
    CHECK(InstanceCounter::instances==1);
    v.emplace<CopyCounter>();
    CHECK(v.index()==1);
    CHECK(InstanceCounter::instances==0);
    CHECK(std::get<CopyCounter>(v).copy_construct==0);
    CHECK(std::get<CopyCounter>(v).move_construct==0);
    CHECK(std::get<CopyCounter>(v).copy_assign==0);
    CHECK(std::get<CopyCounter>(v).move_assign==0);
}

TEST_CASE("emplace_by_index_to_empty") {
    const char* const msg="hello";
    variant<int,char const*,std::string> v;
    v.emplace<2>(msg);
    CHECK(v.index()==2);
    CHECK(std::get<2>(v)==msg);
}

TEST_CASE("emplace_by_index_to_same_type") {
    CopyCounter cc;
    variant<int,CopyCounter> v(cc);
    v.emplace<1>();
    CHECK(v.index()==1);
    CHECK(std::get<CopyCounter>(v).copy_construct==0);
    CHECK(std::get<CopyCounter>(v).move_construct==0);
    CHECK(std::get<CopyCounter>(v).copy_assign==0);
    CHECK(std::get<CopyCounter>(v).move_assign==0);
}

TEST_CASE("emplace_by_index_of_diff_types_destroys_old") {
    variant<InstanceCounter,CopyCounter> v{InstanceCounter()};
    CHECK(v.index()==0);
    CHECK(InstanceCounter::instances==1);
    v.emplace<1>();
    CHECK(v.index()==1);
    CHECK(InstanceCounter::instances==0);
    CHECK(std::get<CopyCounter>(v).copy_construct==0);
    CHECK(std::get<CopyCounter>(v).move_construct==0);
    CHECK(std::get<CopyCounter>(v).copy_assign==0);
    CHECK(std::get<CopyCounter>(v).move_assign==0);
}

TEST_CASE("swap_same_type") {
    variant<int,CopyCounter> v{CopyCounter()};
    CHECK(std::get<CopyCounter>(v).copy_construct==0);
    CHECK(std::get<CopyCounter>(v).move_construct==1);
    CHECK(std::get<CopyCounter>(v).copy_assign==0);
    CHECK(std::get<CopyCounter>(v).move_assign==0);

    CopyCounter cc;
    variant<int,CopyCounter> v2{cc};
    CHECK(std::get<CopyCounter>(v2).copy_construct==1);
    CHECK(std::get<CopyCounter>(v2).move_construct==0);
    CHECK(std::get<CopyCounter>(v2).copy_assign==0);
    CHECK(std::get<CopyCounter>(v2).move_assign==0);
    v.swap(v2);

    CHECK(v.index()==1);
    CHECK(v2.index()==1);
    CHECK(std::get<CopyCounter>(v).copy_construct==1);
    CHECK(std::get<CopyCounter>(v).move_construct==0);
    CHECK(std::get<CopyCounter>(v).copy_assign==0);
    CHECK(std::get<CopyCounter>(v).move_assign==1);
    CHECK(std::get<CopyCounter>(v2).copy_construct==0);
    CHECK(std::get<CopyCounter>(v2).move_construct==2);
    CHECK(std::get<CopyCounter>(v2).copy_assign==0);
    CHECK(std::get<CopyCounter>(v2).move_assign==1);
}

TEST_CASE("swap_different_types") {
    variant<int,CopyCounter> v{CopyCounter()};
    CHECK(std::get<CopyCounter>(v).copy_construct==0);
    CHECK(std::get<CopyCounter>(v).move_construct==1);
    CHECK(std::get<CopyCounter>(v).copy_assign==0);
    CHECK(std::get<CopyCounter>(v).move_assign==0);

    variant<int,CopyCounter> v2{42};
    v.swap(v2);

    CHECK(v.index()==0);
    CHECK(v2.index()==1);
    CHECK(std::get<int>(v)==42);
    CHECK(std::get<CopyCounter>(v2).copy_construct==0);
    std::cout<<"move count="<<std::get<CopyCounter>(v2).move_construct<<std::endl;
    CHECK(std::get<CopyCounter>(v2).move_construct<=3);
    CHECK(std::get<CopyCounter>(v2).move_construct>1);
    CHECK(std::get<CopyCounter>(v2).copy_assign==0);
    CHECK(std::get<CopyCounter>(v2).move_assign==0);
    v.swap(v2);
    CHECK(v2.index()==0);
    CHECK(v.index()==1);
    CHECK(std::get<int>(v2)==42);
    CHECK(std::get<CopyCounter>(v).copy_construct==0);
    CHECK(std::get<CopyCounter>(v).move_construct==4);
    CHECK(std::get<CopyCounter>(v).copy_assign==0);
    CHECK(std::get<CopyCounter>(v).move_assign==0);
}

TEST_CASE("assign_empty_to_empty") {
    variant<int> v1,v2;
    empty_variant(v1);
    empty_variant(v2);
    v1=v2;
    CHECK(v1.index()==-1);
    CHECK(v2.index()==-1);
}

TEST_CASE("swap_empties") {
    variant<int> v1,v2;
    empty_variant(v1);
    empty_variant(v2);
    v1.swap(v2);
    CHECK(v1.index()==-1);
    CHECK(v2.index()==-1);
}

struct VisitorIS {
    int & i;
    std::string & s;

    void operator()(int arg){
        i=arg;
    }
    void operator()(std::string const& arg){
        s=arg;
    }
};

TEST_CASE("visit") {
    variant<int,std::string> v(42);

    int i=0;
    std::string s;
    VisitorIS visitor{i,s};
    std::visit(visitor,v);
    CHECK(i==42);
    i=0;
    v=std::string("hello");
    std::visit(visitor,v);
    CHECK(s=="hello");
    try{
        variant<int,std::string> v2;
        empty_variant(v2);
        std::visit(visitor,v2);
        CHECK(!"Visiting empty should throw");
    } catch(std::bad_variant_access) {
        //
    }
}

TEST_CASE("reference_members") {
    int i=42;
    variant<int&> v(std::in_place_index_t<0>{},i);

    CHECK(v.index()==0);
    CHECK(&std::get<int&>(v)==&i);
    CHECK(&std::get<0>(v)==&i);
}

TEST_CASE("equality") {
    variant<int,double,std::string> v(42);
    variant<int,double,std::string> v2(4.2);
    variant<int,double,std::string> v3(std::string("42"));

    CHECK(v==v);
    CHECK(v!=v2);
    CHECK(v!=v3);
    CHECK(v2==v2);
    CHECK(v3==v3);
    variant<int,double,std::string> v4(v);
    CHECK(v==v4);
    v4=std::move(v2);
    CHECK(v4!=v2);
    CHECK(v2==v2);
    CHECK(v!=v2);
    v2=3;
    CHECK(v!=v2);
}

TEST_CASE("less_than") {
    variant<int,double,std::string> v(42);
    variant<int,double,std::string> v2(4.2);
    variant<int,double,std::string> v3(std::string("42"));

    CHECK(!(v<v));
    CHECK(v>=v);
    CHECK(v<v2);
    CHECK(v<v3);
    CHECK(v2<v3);
    variant<int,double,std::string> v4(v);
    CHECK(!(v4<v));
    CHECK(!(v<v4));
    v4=std::move(v2);
    CHECK(v2<v4);
    CHECK(v2<v);
    CHECK(v2<v3);
    v2=99;
    CHECK(v<v2);
    CHECK(v2<v4);
    CHECK(v2<v3);
}

TEST_CASE("constexpr_variant") {
    constexpr variant<int> v(42);
    constexpr int i=std::get<int>(v);
    CHECK(i==42);
    constexpr variant<int> v2(std::in_place_index_t<0>{},42);
    constexpr int i2 = std::get<int>(v2);
    CHECK(i2==42);
    constexpr variant<int> v3(std::in_place_type_t<int>{},42);
    constexpr int i3 = std::get<int>(v3);
    CHECK(i3==42);
    constexpr variant<int,double> v4(4.2);
    constexpr int i4=v4.index();
    CHECK(i4==1);
    constexpr bool b4 = v4.valueless_by_exception();
    CHECK(!b4);
    constexpr variant<int,double> v5;
    constexpr int i5=v5.index();
    CHECK(i5==0);
    constexpr bool b5=v5.valueless_by_exception();
    CHECK(!b5);
}

struct VisitorISD
{
    int& i;
    std::string& s;
    double& d;
    int& i2;

    void operator()(int arg,double d_){
        i=arg;
        d=d_;
    }
    void operator()(std::string const& arg,double d_){
        s=arg;
        d=d_;
    }
    void operator()(int arg,int i2_){
        i=arg;
        i2=i2_;
    }
    void operator()(std::string const& arg,int i2_){
        s=arg;
        i2=i2_;
    }
};

template<size_t>
struct MarkerArg{};

struct ThreeVariantVisitor{

    size_t a1,a2,a3;

    ThreeVariantVisitor():
        a1(0),a2(0),a3(0)
    {}

    template<size_t A1,size_t A2,size_t A3>
    void operator()(MarkerArg<A1>,MarkerArg<A2>,MarkerArg<A3>){
        a1=A1;
        a2=A2;
        a3=A3;
    }
};

TEST_CASE("multivisitor") {
    variant<int,char,std::string> v(42);
    variant<double,int> v2(4.2);

    int i=0;
    std::string s;
    double d=0;
    int i2=0;
    VisitorISD visitor{i,s,d,i2};

    // FIXME
//     std::visit(visitor, v, v2);
//     CHECK(i==42);
//     CHECK(s=="");
//     CHECK(d==4.2);
//     CHECK(i2==0);
//     i=0;
//     d=0;
//     v=std::string("hello");
//     CHECK(v.index()==2);
//     v2=37;
//     std::visit(visitor,v,v2);
//     CHECK(i==0);
//     CHECK(s=="hello");
//     CHECK(d==0);
//     CHECK(i2==37);

    variant<double,int> v3;
    empty_variant(v3);

    try {
        // FIXME
//         std::visit(visitor,v,v3);
//         CHECK(!"Visiting empty should throw");
    } catch(std::bad_variant_access) {
        //
    }

    variant<MarkerArg<0>,MarkerArg<1> > mv1{MarkerArg<1>()};
    variant<MarkerArg<10>,MarkerArg<11>,MarkerArg<21> > mv2{MarkerArg<21>()};
    variant<MarkerArg<100>, MarkerArg<101>, MarkerArg<201>, MarkerArg<301>>
        mv3{MarkerArg<100>()};

    ThreeVariantVisitor tvv;

    // FIXME
//     std::visit(tvv,mv1,mv2,mv3);
//
//     CHECK(tvv.a1==1);
//     CHECK(tvv.a2==21);
//     CHECK(tvv.a3==100);
}

TEST_CASE("sizes") {
//     std::cout<<"variant<char>:"<<sizeof(variant<char>)<<std::endl;
//     std::cout<<"variant<char,int>:"<<sizeof(variant<char,int>)<<std::endl;
//     std::cout<<"int:"<<sizeof(int)<<std::endl;
//     std::cout<<"variant<char,double>:"<<sizeof(variant<char,double>)<<std::endl;
//     std::cout<<"double:"<<sizeof(double)<<std::endl;
//     std::cout<<"variant<char,std::string>:"<<sizeof(variant<char,std::string>)<<std::endl;
//     std::cout<<"std::string:"<<sizeof(std::string)<<std::endl;
//     std::cout<<"variant<char,std::pair<int,int>>:"<<sizeof(variant<char,std::pair<int,int>>)<<std::endl;
//     std::cout<<"std::pair<int,int>:"<<sizeof(std::pair<int,int>)<<std::endl;
//     std::cout<<"variant<char,std::pair<char,char>>:"<<sizeof(variant<char,std::pair<char,char>>)<<std::endl;
//     std::cout<<"std::pair<char,char>:"<<sizeof(std::pair<char,char>)<<std::endl;
// std::cout<<"variant<char,std::pair<double,char>>:"<<sizeof(variant<char,std::pair<double,char>>)<<std::endl;
//     std::cout<<"std::pair<double,char>:"<<sizeof(std::pair<double,char>)<<std::endl;
//     std::cout<<"variant<double,std::tuple<int,int,int>>:"<<sizeof(variant<double,std::tuple<int,int,int>>)<<std::endl;
//     std::cout<<"std::tuple<int,int,int>:"<<sizeof(std::tuple<int,int,int>)<<std::endl;
}

TEST_CASE("duplicate_types")
{
    variant<int,int> v(42);
    // CHECK(std::get<int>(v)==42);
    CHECK(v.index()==0);
    CHECK(std::get<0>(v)==42);

    variant<int,int> v2(std::in_place_index_t<1>{},42);
    CHECK(v2.index()==1);
    CHECK(std::get<1>(v2)==42);
    // CHECK(std::get<int>(v2)==42);
}

struct NonMovable
{
    int i;
    NonMovable():i(42){}
    NonMovable(NonMovable&&)=delete;
    NonMovable& operator=(NonMovable&&)=delete;
};

TEST_CASE("non_movable_types")
{
    variant<NonMovable> v{std::in_place_index_t<0>{}};
    CHECK(std::get<0>(v).i==42);
    std::get<0>(v).i=37;
    v.emplace<NonMovable>();
    CHECK(std::get<0>(v).i==42);
}

TEST_CASE("direct_init_reference_member")
{
    int i=42;
    variant<int&> v(i);
    CHECK(&std::get<int&>(v)==&i);
}

TEST_CASE("reference_types_preferred_for_lvalue")
{
    int i=42;
    variant<int,int&> v(i);
    CHECK(v.index()==1);

    variant<int,int&> v2(42);
    CHECK(v2.index()==0);
}

TEST_CASE("construction_with_conversion")
{
    variant<int,std::string> v("hello");
    CHECK(v.index()==1);
    CHECK(std::get<1>(v)=="hello");
}

TEST_CASE("assignment_with_conversion")
{
    variant<int,std::string> v;
    v="hello";
    CHECK(v.index()==1);
    CHECK(std::get<1>(v)=="hello");
}

TEST_CASE("visitor_with_non_void_return")
{
    variant<int> v(42);
    // FIXME
    //CHECK(visit([](int i) {return i * 2;},v) == 84);
}

TEST_CASE("multi_visitor_with_non_void_return")
{
    variant<int> v(42);
    variant<double> v2(4.2);

    // FIXME
//     CHECK(std::visit([](int i, double j) {return i+j;},v,v2)==46.2);
}

using vv = variant<std::vector<int>, std::vector<double>>;
unsigned foo(vv v)
{
    return v.index();
}

TEST_CASE("initialization_with_initializer_list")
{
    variant<std::vector<int>> v{1,2,3,4};
    CHECK(v.index()==0);
    CHECK(std::get<0>(v).size()==4);

    CHECK(foo({ 1, 2, 3 })==0); // OK
    CHECK(foo({ 1.2, 3.4, 5.6 })==1); // OK

    // variant< char, std::string > q { { 999 } }; // error: can’t deduce T.
    variant< char, std::string > r = { 999 }; // valid, but overflows.
    variant< char, std::string > s = { 'a', 'b', 'c' }; // error.
}

struct vector_type;
typedef variant<int,double,std::string,vector_type> JSON;

struct vector_type
{
    std::vector<JSON> vec;
    template<typename T>
    vector_type(std::initializer_list<T> list):
        vec(list.begin(),list.end()){}
    vector_type(std::initializer_list<JSON> list):
        vec(list.begin(),list.end()){}
};

TEST_CASE("json") {
    JSON v1{1};
    JSON v2{4.2};
    JSON v3{"hello"};
    JSON v4{{1,2,3}};
    CHECK(v4.index()==3);
    JSON v5{vector_type{1,2,"hello"}};
}

TEST_CASE("nothrow_assign_to_variant_holding_type_with_throwing_move_ok") {
    variant<ThrowingCopy,int> v{std::in_place_index_t<0>{}};
    v=42;
    CHECK(v.index()==1);
    CHECK(std::get<1>(v)==42);
}

TEST_CASE("maybe_throw_assign_to_variant_holding_type_with_throwing_move_ok") {
    variant<ThrowingCopy,std::string> v{std::in_place_index_t<0>{}};
    v="hello";
    CHECK(v.index()==1);
    CHECK(std::get<1>(v)=="hello");
}

TEST_CASE("throwing_assign_from_type_leaves_variant_unchanged") {
    variant<ThrowingCopy,std::string> v{"hello"};
    try{
        v=ThrowingCopy();
        CHECK(!"Should throw");
    }
    catch(CopyError&){}
    CHECK(v.index()==1);
    CHECK(std::get<1>(v)=="hello");
}

TEST_CASE("can_emplace_nonmovable_type_when_other_nothrow_movable") {
    variant<std::string,NonMovable> v{"hello"};
    v.emplace<1>();
    CHECK(v.index()==1);
}

struct NonMovableThrower
{
    NonMovableThrower(int i)
    {
        if(i==42)
            throw CopyError();
    }

    NonMovableThrower(NonMovableThrower&&)=delete;
    NonMovableThrower& operator=(NonMovableThrower&&)=delete;
};

TEST_CASE("throwing_emplace_from_nonmovable_type_leaves_variant_empty")
{
    variant<NonMovableThrower,std::string> v{"hello"};
    try{
        v.emplace<NonMovableThrower>(42);
        CHECK(!"Should throw");
    }
    catch(CopyError&){}
    CHECK(v.index()==-1);
}

TEST_CASE("throwing_emplace_when_stored_type_can_throw_leaves_variant_empty")
{
    variant<NonMovableThrower,ThrowingCopy> v{
        std::in_place_type_t<ThrowingCopy>{}};
    std::get<1>(v).data=21;

    try {
        v.emplace<NonMovableThrower>(42);
        CHECK(!"Should throw");
    } catch(CopyError&) {
        //
    }

    CHECK(v.index()==-1);
}

struct MayThrowA
{
    int data;
    MayThrowA(int i):
        data(i){}
    MayThrowA(MayThrowA const& other):
        data(other.data){}
};

struct MayThrowB
{
    int data;
    MayThrowB(int i):
        data(i){}
    MayThrowB(MayThrowB const& other):
        data(other.data){}
};

TEST_CASE("after_assignment_which_triggers_backup_storage_can_assign_variant")
{
    variant<MayThrowA,MayThrowB> v{MayThrowA(23)};
    v.emplace<MayThrowB>(42);
    CHECK(v.index()==1);
    CHECK(std::get<1>(v).data==42);
    variant<MayThrowA,MayThrowB> v2=v;
    CHECK(v2.index()==1);
    CHECK(std::get<1>(v2).data==42);
    v=MayThrowA(23);
    CHECK(v.index()==0);
    CHECK(std::get<0>(v).data==23);
    v2=v;
    CHECK(v2.index()==0);
    CHECK(std::get<0>(v2).data==23);
    v2=MayThrowB(19);
    CHECK(v2.index()==1);
    CHECK(std::get<1>(v2).data==19);
    v=v2;
    CHECK(v2.index()==1);
    CHECK(std::get<1>(v2).data==19);
}

TEST_CASE("backup_storage_and_local_backup")
{
    variant<std::string,ThrowingCopy> v{"hello"};
    CHECK(v.index()==0);
    CHECK(std::get<0>(v)=="hello");
    try{
        v=ThrowingCopy();
        CHECK(!"Should throw");
    }
    catch(CopyError&){}
    CHECK(v.index()==0);
    CHECK(std::get<0>(v)=="hello");
}

struct LargeNoExceptMovable
{
    char buf[512];

    LargeNoExceptMovable() noexcept{}
    LargeNoExceptMovable(LargeNoExceptMovable&&) noexcept{}
    LargeNoExceptMovable(LargeNoExceptMovable const&&) noexcept{}
    LargeNoExceptMovable& operator=(LargeNoExceptMovable&&) noexcept{
        return *this;
    }
    LargeNoExceptMovable& operator=(LargeNoExceptMovable const&&) noexcept{
        return *this;
    }
};

TEST_CASE("large_noexcept_movable_and_small_throw_movable")
{
    variant<LargeNoExceptMovable,MayThrowA,MayThrowB> v{
        LargeNoExceptMovable()};
    v=MayThrowB(21);
    v=LargeNoExceptMovable();
    v=MayThrowA(12);
    CHECK(sizeof(v)<(2*sizeof(LargeNoExceptMovable)));
}

struct LargeMayThrowA
{
    char dummy[16];
    LargeMayThrowA();
    LargeMayThrowA(LargeMayThrowA const&){}
};

TEST_CASE("construct_small_with_large_throwables")
{
    variant<std::string,LargeMayThrowA,MayThrowB> v{
        MayThrowB(12)};
    v="hello";
    CHECK(v.index()==0);
    CHECK(std::get<0>(v)=="hello");
    v=MayThrowB(42);
    std::cout<<"size of v="<<sizeof(v)<<std::endl;
    std::cout<<"size of string="<<sizeof(std::string)<<std::endl;
    std::cout<<"size of LargeMayThrowA="<<sizeof(LargeMayThrowA)<<std::endl;
    std::cout<<"size of MayThrowB="<<sizeof(MayThrowB)<<std::endl;
}

TEST_CASE("if_emplace_throws_variant_is_valueless")
{
    variant<int> v;
    CHECK(!v.valueless_by_exception());
    CHECK(v.index()==0);
    try{
        v.emplace<0>(ThrowingConversion());
        CHECK(!"Conversion should throw");
    }
    catch(...){}
    CHECK(v.index()==-1);
    CHECK(v.valueless_by_exception());
}

#ifndef __MINGW32__
TEST_CASE("properties")
{
    static_assert(!std::is_default_constructible<variant<>>::value);
    static_assert(std::is_copy_constructible<variant<>>::value);  // or should this be false?
    static_assert(std::is_copy_constructible<variant<int>>::value);
    static_assert(!std::is_copy_constructible<variant<std::mutex,int>>::value);
    static_assert(!std::is_move_constructible<variant<std::mutex,int>>::value);
    static_assert( std::is_nothrow_move_constructible<variant<std::string>>::value);
    static_assert(!std::is_move_assignable<variant<std::mutex,int>>::value);
    static_assert(!std::is_copy_assignable<variant<std::mutex,int>>::value);
    static_assert(std::is_move_assignable<variant<std::string,int>>::value);
    static_assert(std::is_copy_assignable<variant<std::string,int>>::value);
    static_assert(std::is_nothrow_move_assignable<variant<std::string,int>>::value);
    static_assert(std::is_move_assignable<variant<ThrowingCopy,int>>::value);
    static_assert(!std::is_nothrow_move_assignable<variant<ThrowingCopy,int>>::value);
    static_assert(!noexcept(variant<ThrowingCopy,int>().swap(std::declval<variant<ThrowingCopy,int>&>())));
    static_assert(noexcept(variant<int,double>().swap(std::declval<variant<int,double>&>())));
}
#endif

TEST_CASE("variant_of_references") {
    static int i=42;
    constexpr variant<int&> vi(i);
    static_assert(&std::get<0>(vi)==&i);
    constexpr variant<std::string&,int&> vi2(i);
    static_assert(&std::get<1>(vi2)==&i);
    constexpr variant<const int&> vi3(i);
    static_assert(&std::get<0>(vi3)==&i);
}

TEST_CASE("variant_size")
{
    static_assert(std::variant_size<variant<int>>::value==1);
    static_assert(std::variant_size<variant<int,double>>::value==2);
    static_assert(std::variant_size<variant<std::string,int,double>>::value==3);
    static_assert(std::variant_size<variant<int,double,int&,const std::string>>::value==4);
    static_assert(std::variant_size<const variant<int>>::value==1);
    static_assert(std::variant_size<const variant<int,double>>::value==2);
    static_assert(std::variant_size<const variant<std::string,int,double>>::value==3);
    static_assert(std::variant_size<const variant<int,double,int&,const std::string>>::value==4);
    static_assert(std::variant_size<volatile variant<int>>::value==1);
    static_assert(std::variant_size<volatile variant<int,double>>::value==2);
    static_assert(std::variant_size<volatile variant<std::string,int,double>>::value==3);
    static_assert(std::variant_size<volatile variant<int,double,int&,const std::string>>::value==4);
    static_assert(std::variant_size<volatile const variant<int>>::value==1);
    static_assert(std::variant_size<volatile const variant<int,double>>::value==2);
    static_assert(std::variant_size<volatile const variant<std::string,int,double>>::value==3);
    static_assert(std::variant_size<volatile const variant<int,double,int&,const std::string>>::value==4);
}

TEST_CASE("variant_alternative")
{
    static_assert(std::is_same<std::variant_alternative<0,variant<int>>::type,int>::value);
    static_assert(std::is_same<std::variant_alternative<0,variant<int,std::string>>::type,int>::value);
    static_assert(std::is_same<std::variant_alternative<1,variant<int,std::string>>::type,std::string>::value);
    static_assert(std::is_same<std::variant_alternative<0,variant<const int>>::type,const int>::value);
    static_assert(std::is_same<std::variant_alternative<0,variant<int&,std::string>>::type,int&>::value);
    static_assert(std::is_same<std::variant_alternative<2,variant<int,std::string,const double&>>::type,const double&>::value);
    static_assert(std::is_same<std::variant_alternative_t<0,variant<int,std::string,const double&>>,int>::value);

    static_assert(std::is_same<std::variant_alternative<0,const variant<int>>::type,const int>::value);
    static_assert(std::is_same<std::variant_alternative<0,const variant<int,std::string>>::type,const int>::value);
    static_assert(std::is_same<std::variant_alternative<1,const variant<int,std::string>>::type,const std::string>::value);
    static_assert(std::is_same<std::variant_alternative<0,const variant<const int>>::type,const int>::value);
    static_assert(std::is_same<std::variant_alternative<0,const variant<int&,std::string>>::type,int&>::value);
    static_assert(std::is_same<std::variant_alternative<2,const variant<int,std::string,const double&>>::type,const double&>::value);
    static_assert(std::is_same<std::variant_alternative_t<0,const variant<int,std::string,const double&>>,const int>::value);

    static_assert(std::is_same<std::variant_alternative<0,volatile const variant<int>>::type,volatile const int>::value);
    static_assert(std::is_same<std::variant_alternative<0,volatile const variant<int,std::string>>::type,volatile const int>::value);
    static_assert(std::is_same<std::variant_alternative<1,volatile const variant<int,std::string>>::type,volatile const std::string>::value);
    static_assert(std::is_same<std::variant_alternative<0,volatile const variant<const int>>::type,volatile const int>::value);
    static_assert(std::is_same<std::variant_alternative<0,volatile const variant<int&,std::string>>::type,int&>::value);
    static_assert(std::is_same<std::variant_alternative<2,volatile const variant<int,std::string,const double&>>::type,const double&>::value);
    static_assert(std::is_same<std::variant_alternative_t<0,volatile const variant<int,std::string,const double&>>,volatile const int>::value);

    static_assert(std::is_same<std::variant_alternative<0,volatile variant<int>>::type,volatile int>::value);
    static_assert(std::is_same<std::variant_alternative<0,volatile variant<int,std::string>>::type,volatile int>::value);
    static_assert(std::is_same<std::variant_alternative<1,volatile variant<int,std::string>>::type,volatile std::string>::value);
    static_assert(std::is_same<std::variant_alternative<0,volatile variant<const int>>::type,volatile const int>::value);
    static_assert(std::is_same<std::variant_alternative<0,volatile variant<int&,std::string>>::type,int&>::value);
    static_assert(std::is_same<std::variant_alternative<2,volatile variant<int,std::string,const double&>>::type,const double&>::value);
    static_assert(std::is_same<std::variant_alternative_t<0,volatile variant<int,std::string,const double&>>,volatile int>::value);
}

TEST_CASE("void npos")
{
    static_assert(std::variant_npos==(size_t)-1);
    static_assert(std::is_same<decltype(std::variant_npos),const size_t>::value);
}

TEST_CASE("holds_alternative")
{
    constexpr variant<int> vi(42);
    static_assert(std::holds_alternative<int>(vi));
    constexpr variant<int,double> vi2(42);
    static_assert(std::holds_alternative<int>(vi2));
    static_assert(!std::holds_alternative<double>(vi2));
    constexpr variant<int,double> vi3(4.2);
    static_assert(!std::holds_alternative<int>(vi3));
    static_assert(std::holds_alternative<double>(vi3));

    const variant<int,double,std::string> vi4(42);
    CHECK(std::holds_alternative<int>(vi4));
    CHECK(!std::holds_alternative<double>(vi4));
    CHECK(!std::holds_alternative<std::string>(vi4));

    variant<int,double,std::string> vi5("hello42");
    CHECK(!std::holds_alternative<int>(vi5));
    CHECK(!std::holds_alternative<double>(vi5));
    CHECK(std::holds_alternative<std::string>(vi5));
}

TEST_CASE("get_with_rvalues")
{
    int i=42;

    // FIXME
//     static_assert(std::get<0>(variant<int>(42)) == 42);
    static_assert(std::is_same<decltype(std::get<0>(variant<int>(42))),int&&>::value);

    // FIXME
//     static_assert(std::get<int>(variant<int>(42))==42);
    static_assert(std::is_same<decltype(std::get<int>(variant<int>(42))),int&&>::value);

    // FIXME
//     static_assert(std::get<1>(variant<double,int,char>(42))==42);

    static_assert(std::is_same<decltype(std::get<1>(variant<double,int,char>(42))),int&&>::value);

    // FIXME
//     static_assert(std::get<int>(variant<double,int,char>(42))==42);
    static_assert(std::is_same<decltype(std::get<int>(variant<double,int,char>(42))),int&&>::value);

    CHECK(std::get<0>(variant<int&>(i))==42);
    static_assert(std::is_same<decltype(std::get<0>(variant<int&>(i))),int&>::value);
    CHECK(&std::get<0>(variant<int&>(i))==&i);
    CHECK(std::get<int&>(variant<int&>(i))==42);
    static_assert(std::is_same<decltype(std::get<int&>(variant<int&>(i))),int&>::value);
    CHECK(&std::get<int&>(variant<int&>(i))==&i);

    CHECK(std::get<0>(variant<int&&>(std::move(i)))==42);
    static_assert(std::is_same<decltype(std::get<0>(variant<int&&>(std::move(i)))),int&&>::value);
    int&& ir=std::get<0>(variant<int&&>(std::move(i)));
    CHECK(&ir==&i);
    CHECK(std::get<int&&>(variant<int&&>(std::move(i)))==42);
    static_assert(std::is_same<decltype(std::get<int&&>(variant<int&&>(std::move(i)))),int&&>::value);
    int&& ir2=std::get<int&&>(variant<int&&>(std::move(i)));
    CHECK(&ir2==&i);

    variant<int> vi(42);

    int&& ri=std::get<0>(std::move(vi));
    int&& ri2=std::get<int>(std::move(vi));
    CHECK(&ri==&ri2);
    CHECK(&ri==&std::get<0>(vi));

    variant<int,std::string> v2("hello");

    // FIXME
//     std::string&& rs=std::get<1>(std::move(v2));
//     std::string&& rs2=std::get<std::string>(std::move(v2));
//     CHECK(&rs==&rs2);
//     CHECK(&rs==&std::get<1>(v2));
//     CHECK(rs=="hello");
}

TEST_CASE("get_with_const_rvalues")
{
    int i=42;

    static_assert(std::get<0>((const variant<int>)(42))==42);
    static_assert(std::is_same<decltype(std::get<0>((const variant<int>)(42))),const int&&>::value);
    static_assert(std::get<int>((const variant<int>)(42))==42);
    static_assert(std::is_same<decltype(std::get<int>((const variant<int>)(42))),const int&&>::value);

    // FIXME
//     static_assert(std::get<1>((const variant<double,int,char>)(42))==42);
    static_assert(std::is_same<decltype(std::get<1>((const variant<double,int,char>)(42))),const int&&>::value);

    // FIXME
//     static_assert(std::get<int>((const variant<double,int,char>)(42))==42);
    static_assert(std::is_same<decltype(std::get<int>((const variant<double,int,char>)(42))),const int&&>::value);

    CHECK(std::get<0>((const variant<int&>)(i))==42);
    static_assert(std::is_same<decltype(std::get<0>((const variant<int&>)(i))),int&>::value);
    CHECK(&std::get<0>((const variant<int&>)(i))==&i);
    CHECK(std::get<int&>((const variant<int&>)(i))==42);
    static_assert(std::is_same<decltype(std::get<int&>((const variant<int&>)(i))),int&>::value);
    CHECK(&std::get<int&>((const variant<int&>)(i))==&i);

    CHECK(std::get<0>((const variant<int&&>)(std::move(i)))==42);
    static_assert(std::is_same<decltype(std::get<0>((const variant<int&&>)(std::move(i)))),int&&>::value);
    int&& ir=std::get<0>((const variant<int&&>)(std::move(i)));
    CHECK(&ir==&i);
    CHECK(std::get<int&&>((const variant<int&&>)(std::move(i)))==42);
    static_assert(std::is_same<decltype(std::get<int&&>((const variant<int&&>)(std::move(i)))),int&&>::value);
    int&& ir2=std::get<int&&>((const variant<int&&>)(std::move(i)));
    CHECK(&ir2==&i);

    const variant<int> vi(42);

    const int&& ri=std::get<0>(std::move(vi));
    const int&& ri2=std::get<int>(std::move(vi));
    CHECK(&ri==&ri2);
    CHECK(&ri==&std::get<0>(vi));

    const variant<int,std::string> v2("hello");

    // FIXME
//     const std::string&& rs=std::get<1>(std::move(v2));
//     const std::string&& rs2=std::get<std::string>(std::move(v2));
//     CHECK(&rs==&rs2);
//     CHECK(&rs==&std::get<1>(v2));
//     CHECK(rs=="hello");
}

TEST_CASE("get_if") {
    constexpr variant<int> cvi(42);
    constexpr variant<double,int,char> cvidc(42);
    constexpr variant<double,int,char> cvidc2(4.2);

    static_assert(std::get_if<0>(cvi)==&std::get<0>(cvi));
    static_assert(std::get_if<int>(cvi)==&std::get<0>(cvi));

    static_assert(!std::get_if<0>(cvidc));
    static_assert(std::get_if<1>(cvidc)==&std::get<1>(cvidc));
    static_assert(!std::get_if<2>(cvidc));
    static_assert(!std::get_if<double>(cvidc));
    static_assert(std::get_if<int>(cvidc)==&std::get<1>(cvidc));
    static_assert(!std::get_if<char>(cvidc));

    static_assert(std::get_if<double>(cvidc2)==&std::get<0>(cvidc2));
    static_assert(!std::get_if<int>(cvidc2));
    static_assert(!std::get_if<char>(cvidc2));

    variant<int> vi(42);
    variant<double,int,char> vidc(42);
    variant<double,int,char> vidc2(4.2);

    CHECK(std::get_if<0>(vi)==&std::get<0>(vi));
    CHECK(std::get_if<int>(vi)==&std::get<0>(vi));

    CHECK(!std::get_if<0>(vidc));
    CHECK(std::get_if<1>(vidc)==&std::get<1>(vidc));
    CHECK(!std::get_if<2>(vidc));
    CHECK(!std::get_if<double>(vidc));
    CHECK(std::get_if<int>(vidc)==&std::get<1>(vidc));
    CHECK(!std::get_if<char>(vidc));

    CHECK(std::get_if<double>(vidc2)==&std::get<0>(vidc2));
    CHECK(!std::get_if<int>(vidc2));
    CHECK(!std::get_if<char>(vidc2));
}

TEST_CASE("constexpr_comparisons")
{
    constexpr variant<int,double,char> vi(42);
    constexpr variant<int,double,char> vi2(21);
    constexpr variant<int,double,char> vd(2.1);
    constexpr variant<int,double,char> vd2(4.2);

    static_assert(vi==vi);
    static_assert(vi!=vi2);
    static_assert(vi>vi2);
    static_assert(vi>=vi2);
    static_assert(vi2<vi);
    static_assert(vi2<=vi);
    static_assert(vd!=vd2);
    static_assert(vd==vd);
    static_assert(vd<vd2);
    static_assert(vd<=vd2);
    static_assert(vd2>vd);
    static_assert(vd2>=vd);
}

struct Identity
{
    template <typename T> constexpr T operator()(T x) {
        return x;
    }
};

struct Sum
{
    template <typename T, typename U>
    constexpr T operator() (T x, U y) {
        return x + y;
    }
};

TEST_CASE("constexpr_visit")
{
    constexpr variant<int,double> vi(42);
    constexpr variant<int,double> vi2(21);

    // FIXME
//     static_assert(std::visit(Identity(),vi)==42);
//     static_assert(std::visit(Sum(),vi,vi2)==63);
}

TEST_CASE("variant_with_no_types")
{
    static_assert(sizeof(variant<>)>0);
    static_assert(!std::is_default_constructible<variant<>>::value);
}

TEST_CASE("monostate")
{
    static_assert(std::is_trivial<std::monostate>::value);
    static_assert(std::is_nothrow_move_constructible<std::monostate>::value);
    static_assert(std::is_nothrow_copy_constructible<std::monostate>::value);
    static_assert(std::is_nothrow_move_assignable<std::monostate>::value);
    static_assert(std::is_nothrow_copy_assignable<std::monostate>::value);
    constexpr std::monostate m1{},m2{};
    static_assert(m1==m2);
    static_assert(!(m1!=m2));
    static_assert(m1>=m2);
    static_assert(m1<=m2);
    static_assert(!(m1<m2));
    static_assert(!(m1>m2));
}

TEST_CASE("hash")
{
    variant<int,std::string> vi(42);
    variant<int,std::string> vi2(vi);

    std::hash<variant<int,std::string>> h;
    static_assert(noexcept(h(vi)));
    static_assert(std::is_same<decltype(h(vi)),size_t>::value);

    // FIXME
//     CHECK(h{vi} == h{vi2});

    std::monostate m{};
    std::hash<std::monostate> hm;
    static_assert(noexcept(hm(m)));
    static_assert(std::is_same<decltype(hm(m)),size_t>::value);
}

unsigned allocate_count=0;
unsigned deallocate_count=0;

template <typename T> struct CountingAllocator
{
    using value_type = T;
    using is_always_equal = std::true_type;
    using pointer = T *;
    using const_pointer = T const *;
    using reference = T &;
    using const_reference = T const &;


    template <typename U> struct rebind
    {
        using other = CountingAllocator<U>;
    };

    template <typename U> CountingAllocator(CountingAllocator<U> const &) {}

    CountingAllocator() {}

    T *allocate(size_t count)
    {
        ++allocate_count;
        auto *buf = malloc(count * sizeof(T));
        return static_cast<T *>(buf);
    }
    void deallocate(T *p, size_t)
    {
        if (!p)
            return;
        ++deallocate_count;
        free(p);
    }

    void destroy(T *p) { p->~T(); }

    template <class... Args>
    void construct(T* p, Args&&... args){
        ::new (static_cast<void*>(p)) T(std::forward<Args>(args)...);
    }
};

template <typename T, typename U>
constexpr bool operator == (CountingAllocator<T> const &, CountingAllocator<U> const &)
{
   return true;
}

template <typename T, typename U>
constexpr bool operator!=(CountingAllocator<T> const &, CountingAllocator<U> const &)
{
   return false;
}

struct allocatable
{
    bool allocator_supplied=false;
    bool was_moved=false;

    allocatable(){}
    template<typename Alloc>
    allocatable(std::allocator_arg_t,Alloc const&):
        allocator_supplied(true){}

    allocatable(allocatable const&)=default;
    allocatable(allocatable&&):was_moved(true){}

    template<typename Alloc>
    allocatable(std::allocator_arg_t,Alloc const&,allocatable const&):
        allocator_supplied(true){}

    template<typename Alloc>
    allocatable(std::allocator_arg_t,Alloc const&,allocatable &&):
        allocator_supplied(true),was_moved(true){}

};

struct allocatable_no_arg
{
    bool allocator_supplied=false;
    bool was_moved=false;

    allocatable_no_arg(){}

    allocatable_no_arg(allocatable_no_arg const&)=default;
    allocatable_no_arg(allocatable_no_arg&&):
        was_moved(true){}


    template<typename Alloc>
    allocatable_no_arg(Alloc const&):
        allocator_supplied(true){}

    template<typename Alloc>
    allocatable_no_arg(Alloc const&,allocatable_no_arg const&):
        allocator_supplied(true){}

    template<typename Alloc>
    allocatable_no_arg(Alloc const&,allocatable_no_arg &&):
        allocator_supplied(true),was_moved(true){}

};

struct not_allocatable
{
    bool allocator_supplied=false;
    bool was_moved=false;

    not_allocatable(){}
    not_allocatable(not_allocatable const&)=default;
    not_allocatable(not_allocatable&&):
        was_moved(true){}

    template<typename Alloc>
    not_allocatable(std::allocator_arg_t,Alloc const&):
        allocator_supplied(true){}

    template<typename Alloc>
    not_allocatable(std::allocator_arg_t,Alloc const&,not_allocatable const&):
        allocator_supplied(true){}
    template<typename Alloc>
    not_allocatable(Alloc const&,not_allocatable &&):
        allocator_supplied(true),was_moved(true){}
};

namespace std{
template<typename Alloc>
struct uses_allocator<allocatable,Alloc>:
        true_type{};

template<typename Alloc>
struct uses_allocator<allocatable_no_arg,Alloc>:
        true_type{};
}

TEST_CASE("allocator_default_constructor_no_allocator_support") {
    struct MyClass
    {
        int i;
        MyClass() : i(42) {}
    };

    variant<MyClass,std::string> vi{std::allocator_arg_t(),CountingAllocator<MyClass>()};

    CHECK(allocate_count==0);
    CHECK(vi.index()==0);
    CHECK(std::get<0>(vi).i==42);

    variant<not_allocatable,std::string> v3{std::allocator_arg_t(),CountingAllocator<int>()};

    CHECK(allocate_count==0);
    CHECK(v3.index()==0);
    CHECK(!std::get<0>(v3).allocator_supplied);
}

TEST_CASE("allocator_default_constructor_allocator_arg_support")
{
    variant<allocatable,std::string> v2{std::allocator_arg_t(),CountingAllocator<int>()};

    CHECK(allocate_count==0);
    CHECK(v2.index()==0);
    CHECK(std::get<0>(v2).allocator_supplied);
}

TEST_CASE("allocator_default_constructor_no_allocator_arg_support")
{
    variant<allocatable_no_arg,std::string> v2{std::allocator_arg_t(),CountingAllocator<int>()};

    CHECK(allocate_count==0);
    CHECK(v2.index()==0);
    CHECK(std::get<0>(v2).allocator_supplied);
}

TEST_CASE("variant_uses_allocator")
{
    static_assert(
        std::uses_allocator<variant<int,std::string>,CountingAllocator<double>>::value,
        "Variant should use allocator");
}

TEST_CASE("allocator_index_constructor_no_allocator_support")
{
    struct MyClass{
        int i;
        MyClass (): i(42) {}
    };

    variant<MyClass,std::string> vi{std::allocator_arg_t()
        , CountingAllocator<MyClass>{}
        , std::in_place_index_t<0>{}};

    CHECK(allocate_count==0);
    CHECK(vi.index()==0);
    CHECK(std::get<0>(vi).i==42);

    variant<std::string,MyClass> vi2{std::allocator_arg_t()
        , CountingAllocator<MyClass>{}
        , std::in_place_index_t<1>{}};

    CHECK(allocate_count==0);
    CHECK(vi2.index()==1);
    CHECK(std::get<1>(vi2).i==42);


    variant<not_allocatable,std::string> v1{std::allocator_arg_t()
        ,CountingAllocator<int>{}
        , std::in_place_index_t<0>{}};

    CHECK(allocate_count==0);
    CHECK(v1.index()==0);
    CHECK(!std::get<0>(v1).allocator_supplied);

    variant<std::string,not_allocatable> v2{std::allocator_arg_t()
        , CountingAllocator<int>{}
        , std::in_place_index_t<1>{}};

    CHECK(allocate_count==0);
    CHECK(v2.index()==1);
    CHECK(!std::get<1>(v2).allocator_supplied);
}

TEST_CASE("allocator_index_constructor_allocator_arg_support")
{
    variant<allocatable,std::string> v1{
          std::allocator_arg_t()
        , CountingAllocator<int>()
        , std::in_place_index_t<0>{}};

    CHECK(allocate_count==0);
    CHECK(v1.index()==0);
    CHECK(std::get<0>(v1).allocator_supplied);

    variant<std::string,allocatable> v2 {
          std::allocator_arg_t()
        , CountingAllocator<int>()
        , std::in_place_index_t<1>{}};

    CHECK(allocate_count==0);
    CHECK(v2.index()==1);
    CHECK(std::get<1>(v2).allocator_supplied);
}

TEST_CASE("allocator_index_constructor_no_allocator_arg_support")
{
    variant<allocatable_no_arg,std::string> v1{
          std::allocator_arg_t{}
        , CountingAllocator<int>{}
        , std::in_place_index_t<0>{}};

    CHECK(allocate_count==0);
    CHECK(v1.index()==0);
    CHECK(std::get<0>(v1).allocator_supplied);

    variant<std::string,allocatable_no_arg> v2 {
          std::allocator_arg_t{}
        , CountingAllocator<int>{}
        , std::in_place_index_t<1>{}};

    CHECK(allocate_count==0);
    CHECK(v2.index()==1);
    CHECK(std::get<1>(v2).allocator_supplied);
}

TEST_CASE("allocator_type_constructor_no_allocator_support")
{
    struct MyClass
    {
        int i;
        MyClass (): i(42) {}
    };

    variant<MyClass,std::string> vi{std::allocator_arg_t()
        , CountingAllocator<MyClass>{}
        , std::in_place_type_t<MyClass>{}};

    CHECK(allocate_count==0);
    CHECK(vi.index()==0);
    CHECK(std::get<0>(vi).i==42);

    variant<std::string,MyClass> vi2{std::allocator_arg_t{}
        , CountingAllocator<MyClass>{}
        , std::in_place_type_t<MyClass>{}};

    CHECK(allocate_count==0);
    CHECK(vi2.index()==1);
    CHECK(std::get<1>(vi2).i==42);

    variant<not_allocatable,std::string> v1{std::allocator_arg_t{}
        , CountingAllocator<int>{}
        , std::in_place_type_t<not_allocatable>{}};

    CHECK(allocate_count==0);
    CHECK(v1.index()==0);
    CHECK(!std::get<0>(v1).allocator_supplied);

    variant<std::string,not_allocatable> v2{std::allocator_arg_t{}
        , CountingAllocator<int>{}
        , std::in_place_type_t<not_allocatable>{}};

    CHECK(allocate_count==0);
    CHECK(v2.index()==1);
    CHECK(!std::get<1>(v2).allocator_supplied);
}

TEST_CASE("allocator_type_constructor_allocator_arg_support")
{
    variant<allocatable,std::string> v1 {std::allocator_arg_t{}
        , CountingAllocator<int>{}
        , std::in_place_type_t<allocatable>{}};

    CHECK(allocate_count==0);
    CHECK(v1.index()==0);
    CHECK(std::get<0>(v1).allocator_supplied);

    variant<std::string,allocatable> v2{std::allocator_arg_t{}
        , CountingAllocator<int>{}
        , std::in_place_type_t<allocatable>{}};

    CHECK(allocate_count==0);
    CHECK(v2.index()==1);
    CHECK(std::get<1>(v2).allocator_supplied);
}

TEST_CASE("allocator_type_constructor_no_allocator_arg_support")
{
    variant<allocatable_no_arg,std::string> v1 {std::allocator_arg_t{}
        , CountingAllocator<int>{}
        , std::in_place_type_t<allocatable_no_arg>{}};

    CHECK(allocate_count==0);
    CHECK(v1.index()==0);
    CHECK(std::get<0>(v1).allocator_supplied);

    variant<std::string,allocatable_no_arg> v2{std::allocator_arg_t{}
        , CountingAllocator<int>{}
        , std::in_place_type_t<allocatable_no_arg>{}};

    CHECK(allocate_count==0);
    CHECK(v2.index()==1);
    CHECK(std::get<1>(v2).allocator_supplied);
}

TEST_CASE("allocator_copy_constructor_no_allocator_support")
{
    struct MyClass
    {
        int i;
        MyClass (): i(42) {}
    };

    variant<MyClass,std::string> vis{std::in_place_type_t<MyClass>{}};
    variant<MyClass,std::string> vi{std::allocator_arg_t{}
        , CountingAllocator<MyClass>{}
        , vis};

    CHECK(allocate_count==0);
    CHECK(vi.index()==0);
    CHECK(std::get<0>(vi).i==42);

    variant<std::string,MyClass> vis2 {std::in_place_type_t<MyClass>{}};
    variant<std::string,MyClass> vi2 {std::allocator_arg_t{}
        , CountingAllocator<MyClass>{}
        , vis2};

    CHECK(allocate_count == 0);
    CHECK(vi2.index() == 1);
    CHECK(std::get<1>(vi2).i == 42);

    variant<not_allocatable,std::string> v1s{std::in_place_type_t<not_allocatable>{}};
    variant<not_allocatable,std::string> v1{std::allocator_arg_t{}
        , CountingAllocator<int>{}
        ,v1s };

    CHECK(allocate_count==0);
    CHECK(v1.index()==0);
    CHECK(!std::get<0>(v1).allocator_supplied);

    variant<std::string,not_allocatable> v2s{std::in_place_type_t<not_allocatable>{}};
    variant<std::string,not_allocatable> v2{std::allocator_arg_t{}
        , CountingAllocator<int>{}
        , v2s};

    CHECK(allocate_count==0);
    CHECK(v2.index()==1);
    CHECK(!std::get<1>(v2).allocator_supplied);
}

TEST_CASE("allocator_copy_constructor_allocator_arg_support")
{
    variant<allocatable,std::string> v1s{std::in_place_type_t<allocatable>{}};
    variant<allocatable,std::string> v1{std::allocator_arg_t{}
        , CountingAllocator<int>{}
        ,v1s};

    CHECK(allocate_count==0);
    CHECK(v1.index()==0);
    CHECK(std::get<0>(v1).allocator_supplied);

    variant<std::string,allocatable> v2s{std::in_place_index_t<1>{}};
    variant<std::string,allocatable> v2{std::allocator_arg_t{}
        , CountingAllocator<int>{}
        , v2s};

    CHECK(allocate_count==0);
    CHECK(v2.index()==1);
    CHECK(std::get<1>(v2).allocator_supplied);
}

TEST_CASE("allocator_copy_constructor_no_allocator_arg_support")
{
    variant<allocatable_no_arg,std::string> v1s{std::in_place_type_t<allocatable_no_arg>{}};
    variant<allocatable_no_arg,std::string> v1{std::allocator_arg_t{}
        , CountingAllocator<int>{}
        , v1s};

    CHECK(allocate_count==0);
    CHECK(v1.index()==0);
    CHECK(std::get<0>(v1).allocator_supplied);

    variant<std::string,allocatable_no_arg> v2s{std::in_place_type_t<allocatable_no_arg>{}};
    variant<std::string,allocatable_no_arg> v2{
        std::allocator_arg_t(),CountingAllocator<int>(),v2s};

    CHECK(allocate_count==0);
    CHECK(v2.index()==1);
    CHECK(std::get<1>(v2).allocator_supplied);
}

TEST_CASE("allocator_move_constructor_no_allocator_support")
{
    struct MyClass
    {
        int i;
        bool was_moved=false;

        MyClass () : i(42) {}

        MyClass(const MyClass&)=default;
        MyClass(MyClass&&):
            i(42),was_moved(true){}
    };

    variant<MyClass,std::string> vis{std::in_place_type_t<MyClass>{}};
    variant<MyClass,std::string> vi{std::allocator_arg_t{}
        , CountingAllocator<MyClass>{}
        , std::move(vis)};

    CHECK(allocate_count==0);
    CHECK(vi.index()==0);
    CHECK(std::get<0>(vi).i==42);
    CHECK(std::get<0>(vi).was_moved);

    variant<std::string,MyClass> vis2{std::in_place_type_t<MyClass>{}};
    variant<std::string,MyClass> vi2{std::allocator_arg_t{}
        , CountingAllocator<MyClass>{}
        , std::move(vis2)};

    CHECK(allocate_count==0);
    CHECK(vi2.index()==1);
    CHECK(std::get<1>(vi2).i==42);
    CHECK(std::get<1>(vi2).was_moved);

    variant<not_allocatable,std::string> v1s{std::in_place_type_t<not_allocatable>{}};
    variant<not_allocatable,std::string> v1{std::allocator_arg_t{}
        , CountingAllocator<int>{}
        , std::move(v1s)};

    CHECK(allocate_count==0);
    CHECK(v1.index()==0);
    CHECK(!std::get<0>(v1).allocator_supplied);
    CHECK(std::get<0>(v1).was_moved);

    variant<std::string,not_allocatable> v2s{std::in_place_type_t<not_allocatable>{}};
    variant<std::string,not_allocatable> v2{std::allocator_arg_t{}
        , CountingAllocator<int>{}
        , std::move(v2s)};

    CHECK(allocate_count==0);
    CHECK(v2.index()==1);
    CHECK(!std::get<1>(v2).allocator_supplied);
    CHECK(std::get<1>(v2).was_moved);
}

TEST_CASE("allocator_move_constructor_allocator_arg_support")
{
    variant<allocatable,std::string> v1s{std::in_place_type_t<allocatable>{}};
    variant<allocatable,std::string> v1{std::allocator_arg_t{}
        , CountingAllocator<int>{}
        , std::move(v1s)};

    CHECK(allocate_count==0);
    CHECK(v1.index()==0);
    CHECK(std::get<0>(v1).allocator_supplied);
    CHECK(std::get<0>(v1).was_moved);

    variant<std::string,allocatable> v2s{std::in_place_index_t<1>{}};
    variant<std::string,allocatable> v2{std::allocator_arg_t{}
        , CountingAllocator<int>{}
        , std::move(v2s)};

    CHECK(allocate_count==0);
    CHECK(v2.index()==1);
    CHECK(std::get<1>(v2).allocator_supplied);
    CHECK(std::get<1>(v2).was_moved);
}

TEST_CASE("allocator_move_constructor_no_allocator_arg_support")
{
    variant<allocatable_no_arg,std::string> v1s{std::in_place_type_t<allocatable_no_arg>{}};
    variant<allocatable_no_arg,std::string> v1{std::allocator_arg_t{}
        , CountingAllocator<int>{}
        , std::move(v1s)};

    CHECK(allocate_count==0);
    CHECK(v1.index()==0);
    CHECK(std::get<0>(v1).allocator_supplied);
    CHECK(std::get<0>(v1).was_moved);

    variant<std::string,allocatable_no_arg> v2s{std::in_place_type_t<allocatable_no_arg>{}};
    variant<std::string,allocatable_no_arg> v2{std::allocator_arg_t{}
        , CountingAllocator<int>{}
        , std::move(v2s)};

    CHECK(allocate_count==0);
    CHECK(v2.index()==1);
    CHECK(std::get<1>(v2).allocator_supplied);
    CHECK(std::get<1>(v2).was_moved);
}
