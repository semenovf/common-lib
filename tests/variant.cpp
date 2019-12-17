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
        pfs::variant<int, std::string> v;
        CHECK(0 == pfs::get<0>(v));

        constexpr pfs::variant<int> cv;
        static_assert(0 == pfs::get<0>(cv), "");
    }

    // Copy constructor
    {
        // `v`
        pfs::variant<int, std::string> v("hello");
        CHECK("hello" == pfs::get<std::string>(v));

        // `w`
        pfs::variant<int, std::string> w(v);
        CHECK("hello" == pfs::get<std::string>(w));

        // Check `v`
        CHECK("hello" == pfs::get<std::string>(v));

        // `cv`
        constexpr pfs::variant<int, const char *> cv(42);
        static_assert(42 == pfs::get<int>(cv), "");

        // `cw`
        // C++17: constexpr is not usable as variant's copy constructor is defaulted.
        // But in MPARK implementation it is not defaulted.
        /*constexpr*/ pfs::variant<int, const char *> cw(cv);
        /*static_assert*/CHECK(42 == pfs::get<int>(cw));
    }

    // Forwarding constructors
    // Direct
    {
        pfs::variant<int, std::string> v(42);
        CHECK(42 == pfs::get<int>(v));

        constexpr pfs::variant<int, const char *> cv(42);
        static_assert(42 == pfs::get<int>(cv), "");
    }

    // DirectConversion
    {
        pfs::variant<int, std::string> v("42");
        CHECK("42" == pfs::get<std::string>(v));

        constexpr pfs::variant<int, const char *> cv(1.1);
        static_assert(1 == pfs::get<int>(cv), "");
    }

    // CopyInitialization
    {
        pfs::variant<int, std::string> v = 42;
        CHECK(42 == pfs::get<int>(v));

        constexpr pfs::variant<int, const char *> cv = 42;
        static_assert(42 == pfs::get<int>(cv), "");
    }

    // CopyInitializationConversion
    {
        pfs::variant<int, std::string> v = "42";
        CHECK("42" == pfs::get<std::string>(v));

        constexpr pfs::variant<int, const char *> cv = 1.1;
        static_assert(1 == pfs::get<int>(cv), "");
    }

    // In-place constructors

    // IndexDirect
    {
        pfs::variant<int, std::string> v(pfs::in_place_index_t<0>(), 42);
        CHECK(42 == pfs::get<0>(v));

        constexpr pfs::variant<int, const char *> cv(pfs::in_place_index_t<0>{},42);
        static_assert(42 == pfs::get<0>(cv), "");
    }

    // IndexDirectDuplicate
    {
        pfs::variant<int, int> v(pfs::in_place_index_t<0>(), 42);
        CHECK(42 == pfs::get<0>(v));

        constexpr pfs::variant<int, int> cv(pfs::in_place_index_t<0>{}, 42);
        static_assert(42 == pfs::get<0>(cv), "");
    }

    // IndexConversion
    {
        pfs::variant<int, std::string> v(pfs::in_place_index_t<1>(), "42");
        CHECK("42" == pfs::get<1>(v));

        constexpr pfs::variant<int, const char *> cv(pfs::in_place_index_t<0>{}, 1.1);
        static_assert(1 == pfs::get<0>(cv), "");
    }

    // IndexInitializerList
    {
        pfs::variant<int, std::string> v(pfs::in_place_index_t<1>(), {'4', '2'});
        CHECK("42" == pfs::get<1>(v));
    }

    // TypeDirect
    {
        pfs::variant<int, std::string> v(pfs::in_place_type_t<std::string>(), "42");
        CHECK("42" == pfs::get<std::string>(v));

        constexpr pfs::variant<int, const char *> cv(
        pfs::in_place_type_t<int>{}, 42);
        static_assert(42 == pfs::get<int>(cv), "");
    }

    // TypeConversion
    {
        pfs::variant<int, std::string> v(pfs::in_place_type_t<int>(), 42.5);
        CHECK(42 == pfs::get<int>(v));

        constexpr pfs::variant<int, const char *> cv(
        pfs::in_place_type_t<int>{}, 42.5);
        static_assert(42 == pfs::get<int>(cv), "");
    }

    // TypeInitializerList
    {
        pfs::variant<int, std::string> v(pfs::in_place_type_t<std::string>(), {'4', '2'});
        CHECK("42" == pfs::get<std::string>(v));
    }

    // Move Constructor
    {
        // `v`
        pfs::variant<int, std::string> v("hello");
        CHECK("hello" == pfs::get<std::string>(v));
        // `w`
        pfs::variant<int, std::string> w(std::move(v));
        CHECK("hello" == pfs::get<std::string>(w));
        // Check `v`
        CHECK(pfs::get<std::string>(v).empty());

        // `cv`
        constexpr pfs::variant<int, const char *> cv(42);
        static_assert(42 == pfs::get<int>(cv), "");
        // `cw`
        /*constexpr*/ pfs::variant<int, const char *> cw(std::move(cv));
        /*static_assert*/CHECK(42 == pfs::get<int>(cw));
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
        pfs::variant<Obj, int> v, w;
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
        pfs::variant<Obj, int> v(42), w;
        // copy assignment.
        v = w;
    }

    //
    // Forwarding
    //

    // SameType
    {
        pfs::variant<int, std::string> v(101);
        CHECK(101 == pfs::get<int>(v));
        v = 202;
        CHECK(202 == pfs::get<int>(v));
    }

    // SameTypeFwd
    {
        pfs::variant<int, std::string> v(1.1);
        CHECK(1 == pfs::get<int>(v));
        v = 2.2;
        CHECK(2 == pfs::get<int>(v));
    }

    // DiffType
    {
        pfs::variant<int, std::string> v(42);
        CHECK(42 == pfs::get<int>(v));
        v = "42";
        CHECK("42" == pfs::get<std::string>(v));
    }

    // DiffTypeFwd
    {
        pfs::variant<int, std::string> v(42);
        CHECK(42 == pfs::get<int>(v));
        v = "42";
        CHECK("42" == pfs::get<std::string>(v));
    }

    // ExactMatch
    {
        pfs::variant<const char *, std::string> v;
        v = std::string("hello");
        CHECK("hello" == pfs::get<std::string>(v));
    }

    // BetterMatch
    {
        pfs::variant<int, double> v;
        // `char` -> `int` is better than `char` -> `double`
        v = 'x';
        CHECK(static_cast<int>('x') == pfs::get<int>(v));
    }

    // NoMatch
    {
        struct x {};
        static_assert(!std::is_assignable<pfs::variant<int, std::string>, x> {},
                      "variant<int, std::string> v; v = x;");
    }

    // Ambiguous
    {
        static_assert(!std::is_assignable<pfs::variant<short, long>, int> {},
                      "variant<short, long> v; v = 42;");
    }

    // SameTypeOptimization
    {
        pfs::variant<int, std::string> v("hello world!");
        // Check `v`.
        std::string const & x = pfs::get<std::string>(v);
        CHECK("hello world!" == x);

        // Save the "hello world!"'s capacity.
        int capacity = x.capacity();

        // Use `std::string::operator=(const char *)` to assign into `v`.
        v = "hello";

        // Check `v`.
        std::string const & y = pfs::get<std::string>(v);

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
        pfs::variant<Obj, int> v, w;
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
        pfs::variant<Obj, int> v(42), w;
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
        pfs::variant<Obj> v(pfs::in_place_type_t<Obj>(), dtor_called);
    }

    CHECK(dtor_called);
}

////////////////////////////////////////////////////////////////////////////////
//                                  Get                                       //
////////////////////////////////////////////////////////////////////////////////
TEST_CASE("Variant Get") {

    // HoldsAlternative
    {
        pfs::variant<int, std::string> v(42);
//         CHECK(pfs::holds_alternative<0>(v));
//         CHECK(!pfs::holds_alternative<1>(v));
        CHECK(pfs::holds_alternative<int>(v));
        CHECK(!pfs::holds_alternative<std::string>(v));

        constexpr pfs::variant<int, const char *> cv(42);
//         static_assert(pfs::holds_alternative<0>(cv), "");
//         static_assert(!pfs::holds_alternative<1>(cv), "");
        static_assert(pfs::holds_alternative<int>(cv), "");
        static_assert(!pfs::holds_alternative<const char *>(cv), "");
    }


    // MutVarMutType
    {
        pfs::variant<int> v(42);
        CHECK(42 == pfs::get<int>(v));
    }

    // MutVarConstType
    {
        pfs::variant<int const> v(42);
        CHECK(42 == pfs::get<int const>(v));
    }

    // ConstVarMutType
    {
        pfs::variant<int> const v(42);
        CHECK(42 == pfs::get<int>(v));

        constexpr pfs::variant<int> cv(42);
        static_assert(42 == pfs::get<int>(cv), "");
    }

    // ConstVarConstType
    {
        const pfs::variant<const int> v(42);
        CHECK(42 == pfs::get<const int>(v));

        constexpr pfs::variant<const int> cv(42);
        static_assert(42 == pfs::get<const int>(cv), "");
    }

    // MutVarMutType
    {
        pfs::variant<int> v(42);
        CHECK(42 == *pfs::get_if<int>(& v));
    }

    // MutVarConstType
    {
        pfs::variant<const int> v(42);
        CHECK(42 == *pfs::get_if<const int>(& v));
    }

    // ConstVarMutType
    {
        const pfs::variant<int> v(42);
        CHECK(42 == *pfs::get_if<int>(&v));

        static constexpr pfs::variant<int> cv(42);
        static_assert(42 == *pfs::get_if<int>(& cv), "");
    }

    // ConstVarConstType
    {
        const pfs::variant<const int> v(42);
        CHECK(42 == *pfs::get_if<const int>(& v));

        static constexpr pfs::variant<const int> cv(42);
        static_assert(42 == *pfs::get_if<const int>(& cv), "");
    }
}

////////////////////////////////////////////////////////////////////////////////
//                                 Modifiers                                  //
////////////////////////////////////////////////////////////////////////////////
TEST_CASE("Variant Modifiers") {

    // IndexDirect
    {
        pfs::variant<int, std::string> v;
        v.emplace<1>("42");
        CHECK("42" == pfs::get<1>(v));
    }

    // IndexDirectDuplicate
    {
        pfs::variant<int, int> v;
        v.emplace<1>(42);
        CHECK(42 == pfs::get<1>(v));
    }

    // IndexConversion
    {
        pfs::variant<int, std::string> v;
        v.emplace<1>("42");
        CHECK("42" == pfs::get<1>(v));
    }

    // IndexConversionDuplicate
    {
        pfs::variant<int, int> v;
        v.emplace<1>(1.1);
        CHECK(1 == pfs::get<1>(v));
    }

    // IndexInitializerList
    {
        pfs::variant<int, std::string> v;
        v.emplace<1>({'4', '2'});
        CHECK("42" == pfs::get<1>(v));
    }

    // TypeDirect
    {
        pfs::variant<int, std::string> v;
        v.emplace<std::string>("42");
        CHECK("42" == pfs::get<std::string>(v));
    }

    // TypeConversion
    {
        pfs::variant<int, std::string> v;
        v.emplace<int>(1.1);
        CHECK(1 == pfs::get<int>(v));
    }

    // TypeInitializerList
    {
#if __cplusplus >= 201103L
        pfs::variant<int, std::string> v;
        v.emplace<std::string>({'4', '2'});
        CHECK("42" == pfs::get<std::string>(v));
#endif
    }
}

////////////////////////////////////////////////////////////////////////////////
//                                 Comparisons                                //
////////////////////////////////////////////////////////////////////////////////
TEST_CASE("Variant Comparisons") {

    // SameTypeSameValue
    {
        pfs::variant<int, std::string> v(0), w(0);
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
        constexpr pfs::variant<int, const char *> cv(0), cw(0);
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
        pfs::variant<int, std::string> v(0), w(1);
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
        constexpr pfs::variant<int, const char *> cv(0), cw(1);
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
        pfs::variant<int, unsigned int> v(0), w(0u);
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
        constexpr pfs::variant<int, unsigned int> cv(0), cw(0u);
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
        pfs::variant<int, unsigned int> v(0), w(1u);
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
        constexpr pfs::variant<int, unsigned int> cv(0), cw(1u);
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
        pfs::variant<int, std::string> v("hello");
        pfs::variant<int, std::string> w("world");
        // Check `v`.
        CHECK("hello" == pfs::get<std::string>(v));
        // Check `w`.
        CHECK("world" == pfs::get<std::string>(w));
        // Swap.
        using std::swap;
        swap(v, w);
        // Check `v`.
        CHECK("world" == pfs::get<std::string>(v));
        // Check `w`.
        CHECK("hello" == pfs::get<std::string>(w));
    }

    // Different
    {
        pfs::variant<int, std::string> v(42);
        pfs::variant<int, std::string> w("hello");
        // Check `v`.
        CHECK(42 == pfs::get<int>(v));
        // Check `w`.
        CHECK("hello" == pfs::get<std::string>(w));
        // Swap.
        using std::swap;
        swap(v, w);
        // Check `v`.
        CHECK("hello" == pfs::get<std::string>(v));
        // Check `w`.
        CHECK(42 == pfs::get<int>(w));
    }

    // DtorsSame
    {
        size_t v_count = 0;
        size_t w_count = 0;
        {
            pfs::variant<swap1::Obj> v(&v_count), w(&w_count);
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
            pfs::variant<swap2::Obj> v(& v_count), w(& w_count);
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
            pfs::variant<swap3::V, swap3::W> v(pfs::in_place_type_t<swap3::V>(), & v_count);
            pfs::variant<swap3::V, swap3::W> w(pfs::in_place_type_t<swap3::W>(), & w_count);
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
        pfs::variant<int> v(42);
        // Check `v`.
        CHECK(42 == pfs::get<int>(v));
        // Check qualifier.
        CHECK(LRef == pfs::visit(get_qual(), v));
        CHECK(RRef == pfs::visit(get_qual(), std::move(v)));
    }

    // MutVarConstType
    {
        pfs::variant<const int> v(42);
        CHECK(42 == pfs::get<const int>(v));
        // Check qualifier.
        CHECK(ConstLRef == pfs::visit(get_qual(), v));
        CHECK(ConstRRef == pfs::visit(get_qual(), std::move(v)));
    }

    // ConstVarMutType
    {
        const pfs::variant<int> v(42);
        CHECK(42 == pfs::get<int>(v));
        // Check qualifier.
        CHECK(ConstLRef == pfs::visit(get_qual(), v));
        CHECK(ConstRRef == pfs::visit(get_qual(), std::move(v)));
    }

    // ConstVarConstType
    {
        const pfs::variant<const int> v(42);
        CHECK(42 == pfs::get<const int>(v));
        // Check qualifier.
        CHECK(ConstLRef == pfs::visit(get_qual(), v));
        CHECK(ConstRRef == pfs::visit(get_qual(), std::move(v)));
    }

    // Zero
    {
        CHECK("" == pfs::visit(concat{}));
    }

    // Double
    {
        pfs::variant<int, std::string> v("hello"), w("world!");
        CHECK("helloworld!" == pfs::visit(concat {}, v, w));
    }

    // Quintuple
    {
        pfs::variant<int, std::string> v(101), w("+"), x(202), y("="), z(303);
        CHECK("101+202=303" == pfs::visit(concat {}, v, w, x, y, z));
    }

    // Double
    {
        pfs::variant<int, std::string> v("hello");
        pfs::variant<double, const char *> w("world!");
        CHECK("helloworld!" == pfs::visit(concat{}, v, w));
    }

    // Quintuple)
    {
        pfs::variant<int, double> v(101);
        pfs::variant<const char *> w("+");
        pfs::variant<bool, std::string, int> x(202);
        pfs::variant<char, std::string, const char *> y('=');
        pfs::variant<long, short> z(303L);
        CHECK("101+202=303" == pfs::visit(concat {}, v, w, x, y, z));
    }
}
