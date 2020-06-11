////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 Vladislav Trifochkin
//
// This file is part of [pfs-common](https://github.com/semenovf/pfs-common) library.
//
// Changelog:
//      2019.12.17 Initial version (inspired from https://github.com/mpark/variant)
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/variant.hpp"
#include <string>
#include <utility>

////////////////////////////////////////////////////////////////////////////////
//                             Constructors                                   //
////////////////////////////////////////////////////////////////////////////////
TEST_CASE("Variant Constructors") {

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
        // But in MPARK implementation it is not defaulted.
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
        std::variant<int, std::string> v(std::in_place_index_t<1>(), "42");
        CHECK("42" == std::get<1>(v));

        constexpr std::variant<int, const char *> cv(std::in_place_index_t<0>{}, 1.1);
        static_assert(1 == std::get<0>(cv), "");
    }

    // IndexInitializerList
    {
        std::variant<int, std::string> v(std::in_place_index_t<1>(), {'4', '2'});
        CHECK("42" == std::get<1>(v));
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
        std::variant<int, std::string> v(std::in_place_type_t<std::string>(), {'4', '2'});
        CHECK("42" == std::get<std::string>(v));
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
        CHECK(std::get<std::string>(v).empty());

        // `cv`
        constexpr std::variant<int, const char *> cv(42);
        static_assert(42 == std::get<int>(cv), "");
        // `cw`
        /*constexpr*/ std::variant<int, const char *> cw(std::move(cv));
        /*static_assert*/CHECK(42 == std::get<int>(cw));
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
        v = 'x';
        CHECK(static_cast<int>('x') == std::get<int>(v));
    }

    // NoMatch
    {
        struct x {};
        static_assert(!std::is_assignable<std::variant<int, std::string>, x> {},
                      "variant<int, std::string> v; v = x;");
    }

    // Ambiguous
    {
        static_assert(!std::is_assignable<std::variant<short, long>, int> {},
                      "variant<short, long> v; v = 42;");
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
//         CHECK(std::holds_alternative<0>(v));
//         CHECK(!std::holds_alternative<1>(v));
        CHECK(std::holds_alternative<int>(v));
        CHECK(!std::holds_alternative<std::string>(v));

        constexpr std::variant<int, const char *> cv(42);
//         static_assert(std::holds_alternative<0>(cv), "");
//         static_assert(!std::holds_alternative<1>(cv), "");
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
        std::variant<int const> v(42);
        CHECK(42 == std::get<int const>(v));
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
        const std::variant<const int> v(42);
        CHECK(42 == std::get<const int>(v));

        constexpr std::variant<const int> cv(42);
        static_assert(42 == std::get<const int>(cv), "");
    }

    // MutVarMutType
    {
        std::variant<int> v(42);
        CHECK(42 == *std::get_if<int>(& v));
    }

    // MutVarConstType
    {
        std::variant<const int> v(42);
        CHECK(42 == *std::get_if<const int>(& v));
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
        const std::variant<const int> v(42);
        CHECK(42 == *std::get_if<const int>(& v));

        static constexpr std::variant<const int> cv(42);
        static_assert(42 == *std::get_if<const int>(& cv), "");
    }
}

////////////////////////////////////////////////////////////////////////////////
//                                 Modifiers                                  //
////////////////////////////////////////////////////////////////////////////////
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
        v.emplace<1>({'4', '2'});
        CHECK("42" == std::get<1>(v));
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
#if __cplusplus >= 201103L
        std::variant<int, std::string> v;
        v.emplace<std::string>({'4', '2'});
        CHECK("42" == std::get<std::string>(v));
#endif
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

#if __cplusplus >= 201103L && (!defined(__GNUC__) || __GNUC__ >= 5)
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

#if __cplusplus >= 201103L && (!defined(__GNUC__) || __GNUC__ >= 5)
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

#if __cplusplus >= 201103L && (!defined(__GNUC__) || __GNUC__ >= 5)
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

#if __cplusplus >= 201103L && (!defined(__GNUC__) || __GNUC__ >= 5)
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
        // Check qualifier.
        CHECK(LRef == std::visit(get_qual(), v));
        CHECK(RRef == std::visit(get_qual(), std::move(v)));
    }

    // MutVarConstType
    {
        std::variant<const int> v(42);
        CHECK(42 == std::get<const int>(v));
        // Check qualifier.
        CHECK(ConstLRef == std::visit(get_qual(), v));
        CHECK(ConstRRef == std::visit(get_qual(), std::move(v)));
    }

    // ConstVarMutType
    {
        const std::variant<int> v(42);
        CHECK(42 == std::get<int>(v));
        // Check qualifier.
        CHECK(ConstLRef == std::visit(get_qual(), v));
        CHECK(ConstRRef == std::visit(get_qual(), std::move(v)));
    }

    // ConstVarConstType
    {
        const std::variant<const int> v(42);
        CHECK(42 == std::get<const int>(v));
        // Check qualifier.
        CHECK(ConstLRef == std::visit(get_qual(), v));
        CHECK(ConstRRef == std::visit(get_qual(), std::move(v)));
    }

    // Zero
    {
        CHECK("" == std::visit(concat{}));
    }

    // Double
    {
        std::variant<int, std::string> v("hello"), w("world!");
        CHECK("helloworld!" == std::visit(concat {}, v, w));
    }

    // Quintuple
    {
        std::variant<int, std::string> v(101), w("+"), x(202), y("="), z(303);
        CHECK("101+202=303" == std::visit(concat {}, v, w, x, y, z));
    }

    // Double
    {
        std::variant<int, std::string> v("hello");
        std::variant<double, const char *> w("world!");
        CHECK("helloworld!" == std::visit(concat{}, v, w));
    }

    // Quintuple)
    {
        std::variant<int, double> v(101);
        std::variant<const char *> w("+");
        std::variant<bool, std::string, int> x(202);
        std::variant<char, std::string, const char *> y('=');
        std::variant<long, short> z(303L);
        CHECK("101+202=303" == std::visit(concat {}, v, w, x, y, z));
    }
}
