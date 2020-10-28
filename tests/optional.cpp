////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of [pfs-common](https://github.com/semenovf/pfs-common) library.
//
// Changelog:
//      2020.06.12 Initial version (inspired from https://github.com/tcbrindle/cpp17_headers)
//
// Copyright (C) 2011 - 2016 Andrzej Krzemienski.
//
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// The idea and interface is based on Boost.Optional library
// authored by Fernando Luis Cacciola Carballal
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/optional.hpp"
#include <complex>
#include <functional>
#include <memory>
#include <string>
#include <utility>

#if defined(PFS_NO_STD_OPTIONAL)
    using pfs::optional;
    using pfs::bad_optional_access;
    using pfs::nullopt;
    using pfs::in_place;
    using pfs::make_optional;
    using pfs::constexpr_move;
#else
    using std::optional;
    using std::bad_optional_access;
    using std::nullopt;
    using std::in_place;
    using std::make_optional;
    using std::constexpr_move;
#endif

TEST_CASE("basic") {
    constexpr optional<int> empty{};

    static_assert(!empty, "");
    CHECK_THROWS_AS(empty.value(), bad_optional_access);

    constexpr optional<int> opt{3};
    CHECK(opt);
    static_assert(opt.value() == 3, "");
}

/* This file is taken from the upstream stx::optional implementation. It makes
 * assumptions about the implementation and will not work with std::optional
 * or std::experimental::optional from standard libraries. That is, you must
 * define STX_NO_STD_OPTIONAL before compiling this file.
 *
 * It is included here to ensure that we don't make any changes from upstream
 * which cause breakage.
 */

enum State
{
    sDefaultConstructed,
    sValueCopyConstructed,
    sValueMoveConstructed,
    sCopyConstructed,
    sMoveConstructed,
    sMoveAssigned,
    sCopyAssigned,
    sValueCopyAssigned,
    sValueMoveAssigned,
    sMovedFrom,
    sValueConstructed
};

struct OracleVal
{
    State s;
    int i;
    OracleVal(int i = 0) : s(sValueConstructed), i(i) {}
};

struct Oracle
{
    State s;
    OracleVal val;

    Oracle() : s(sDefaultConstructed) {}
    Oracle(const OracleVal& v) : s(sValueCopyConstructed), val(v) {}
    Oracle(OracleVal&& v) : s(sValueMoveConstructed), val(std::move(v)) {v.s = sMovedFrom;}
    Oracle(const Oracle& o) : s(sCopyConstructed), val(o.val) {}
    Oracle(Oracle&& o) : s(sMoveConstructed), val(std::move(o.val)) {o.s = sMovedFrom;}

    Oracle& operator=(const OracleVal& v) { s = sValueCopyConstructed; val = v; return *this; }
    Oracle& operator=(OracleVal&& v) { s = sValueMoveConstructed; val = std::move(v); v.s = sMovedFrom; return *this; }
    Oracle& operator=(const Oracle& o) { s = sCopyConstructed; val = o.val; return *this; }
    Oracle& operator=(Oracle&& o) { s = sMoveConstructed; val = std::move(o.val); o.s = sMovedFrom; return *this; }
};

struct Guard
{
    std::string val;
    Guard() : val{} {}
    explicit Guard (std::string s, int = 0) : val(s) {}
    Guard (const Guard &) = delete;
    Guard (Guard &&) = delete;
    void operator=(const Guard &) = delete;
    void operator=(Guard &&) = delete;
};

struct ExplicitStr
{
    std::string s;
    explicit ExplicitStr(const char* chp) : s(chp) {};
};

struct Date
{
    int i;
    Date() = delete;
    Date(int i) : i{i} {};
    Date(Date&& d) : i(d.i) { d.i = 0; }
    Date(const Date&) = delete;
    Date& operator=(const Date&) = delete;
    Date& operator=(Date&& d) { i = d.i; d.i = 0; return *this;};
};

inline bool operator == (Oracle const & a, Oracle const & b )
{
    return a.val.i == b.val.i;
}

inline bool operator != (Oracle const & a, Oracle const & b )
{
    return a.val.i != b.val.i;
}

//namespace tr2 = stx;
TEST_CASE("disengaged_ctor") {
    optional<int> o1;
    CHECK(!o1);

    optional<int> o2 = nullopt;
    CHECK(!o2);

    optional<int> o3 = o2;
    CHECK(!o3);

    CHECK(o1 == nullopt);
    CHECK(o1 == optional<int>{});
    CHECK(!o1);
    CHECK(bool(o1) == false);

    CHECK(o2 == nullopt);
    CHECK(o2 == optional<int>{});
    CHECK(!o2);
    CHECK(bool(o2) == false);

    CHECK(o3 == nullopt);
    CHECK(o3 == optional<int>{});
    CHECK(!o3);
    CHECK(bool(o3) == false);

    CHECK(o1 == o2);
    CHECK(o2 == o1);
    CHECK(o1 == o3);
    CHECK(o3 == o1);
    CHECK(o2 == o3);
    CHECK(o3 == o2);
};


TEST_CASE("value_ctor")
{
    OracleVal v;
    optional<Oracle> oo1(v);
    CHECK(oo1 != nullopt);
    CHECK(oo1 != optional<Oracle>{});
    CHECK(oo1 == optional<Oracle>{v});
    CHECK(!!oo1);
    CHECK(bool(oo1));
    // NA: CHECK(oo1->s == sValueCopyConstructed);
//     CHECK(oo1->s == sMoveConstructed); // FIXME
    CHECK(v.s == sValueConstructed);

    optional<Oracle> oo2(std::move(v));
    CHECK(oo2 != nullopt);
    CHECK(oo2 != optional<Oracle>{});
    CHECK(oo2 == oo1);
    CHECK(!!oo2);
    CHECK(bool(oo2));
    // NA: CHECK(oo2->s == sValueMoveConstructed);
//     CHECK(oo2->s == sMoveConstructed); // FIXME
    CHECK(v.s == sMovedFrom);

    {
        OracleVal v;
        optional<Oracle> oo1{in_place, v};
        CHECK(oo1 != nullopt);
        CHECK(oo1 != optional<Oracle>{});
        CHECK(oo1 == optional<Oracle>{v});
        CHECK(!!oo1);
        CHECK(bool(oo1));
        CHECK(oo1->s == sValueCopyConstructed);
        CHECK(v.s == sValueConstructed);

        optional<Oracle> oo2{in_place, std::move(v)};
        CHECK(oo2 != nullopt);
        CHECK(oo2 != optional<Oracle>{});
        CHECK(oo2 == oo1);
        CHECK(!!oo2);
        CHECK(bool(oo2));
        CHECK(oo2->s == sValueMoveConstructed);
        CHECK(v.s == sMovedFrom);
    }
}

TEST_CASE("assignment")
{
    optional<int> oi;
    oi = optional<int>{1};
    CHECK(*oi == 1);

    oi = nullopt;
    CHECK(!oi);

    oi = 2;
    CHECK(*oi == 2);

    oi = {};
    CHECK(!oi);
}

template <class T>
struct MoveAware
{
    T val;
    bool moved;

    MoveAware(T val)
        : val(val)
        , moved(false)
    {}

    MoveAware (MoveAware && rhs)
        : val(rhs.val)
        , moved(rhs.moved)
    {
        rhs.moved = true;
    }

    MoveAware (MoveAware const &) = delete;
    MoveAware & operator = (MoveAware const &) = delete;

    MoveAware & operator = (MoveAware && rhs)
    {
        val = (rhs.val);
        moved = (rhs.moved);
        rhs.moved = true;
        return *this;
    }
};

TEST_CASE("moved_from_state")
{
    // first, test mock:
    MoveAware<int> i{1}, j{2};
    CHECK(i.val == 1);
    CHECK(!i.moved);
    CHECK(j.val == 2);
    CHECK(!j.moved);

    MoveAware<int> k = std::move(i);
    CHECK(k.val == 1);
    CHECK(!k.moved);
    CHECK(i.val == 1);
    CHECK(i.moved);

    k = std::move(j);
    CHECK(k.val == 2);
    CHECK(!k.moved);
    CHECK(j.val == 2);
    CHECK(j.moved);

    // now, test optional
    optional<MoveAware<int>> oi{1}, oj{2};
    CHECK(oi);
    CHECK(!oi->moved);
    CHECK(oj);
    CHECK(!oj->moved);

    optional<MoveAware<int>> ok = std::move(oi);
    CHECK(ok);
    CHECK(!ok->moved);
    CHECK(oi);
    CHECK(oi->moved);

    ok = std::move(oj);
    CHECK(ok);
    CHECK(!ok->moved);
    CHECK(oj);
    CHECK(oj->moved);
};

TEST_CASE("copy_move_ctor_optional_int")
{
    optional<int> oi;
    optional<int> oj = oi;

    CHECK(!oj);
    CHECK(oj == oi);
    CHECK(oj == nullopt);
    CHECK(!bool(oj));

    oi = 1;
    optional<int> ok = oi;
    CHECK(!!ok);
    CHECK(bool(ok));
    CHECK(ok == oi);
    CHECK(ok != oj);
    CHECK(*ok == 1);

    optional<int> ol = std::move(oi);
    CHECK(!!ol);
    CHECK(bool(ol));
    CHECK(ol == oi);
    CHECK(ol != oj);
    CHECK(*ol == 1);
}

TEST_CASE("optional_optional")
{
    optional<optional<int>> oi1 = nullopt;
    CHECK(oi1 == nullopt);
    CHECK(!oi1);

    {
        optional<optional<int>> oi2 {in_place};
        CHECK(oi2 != nullopt);
        CHECK(bool(oi2));
        CHECK(*oi2 == nullopt);
        //CHECK(!(*oi2));
        //std::cout << typeid(**oi2).name() << std::endl;
    }

    {
        optional<optional<int>> oi2 {in_place, nullopt};
        CHECK(oi2 != nullopt);
        CHECK(bool(oi2));
        CHECK(*oi2 == nullopt);
        CHECK(!*oi2);
    }

    {
        optional<optional<int>> oi2 {optional<int>{}};
        CHECK(oi2 != nullopt);
        CHECK(bool(oi2));
        CHECK(*oi2 == nullopt);
        CHECK(!*oi2);
    }

    optional<int> oi;
    auto ooi = make_optional(oi);
    static_assert( std::is_same<optional<optional<int>>, decltype(ooi)>::value, "");
}

TEST_CASE("example_guard")
{
    //FAILS: optional<Guard> ogx(Guard("res1"));
    //FAILS: optional<Guard> ogx = "res1";
    //FAILS: optional<Guard> ogx("res1");
    optional<Guard> oga;                  // Guard is non-copyable (and non-moveable)
    optional<Guard> ogb(in_place, "res1");   // initialzes the contained value with "res1"
    CHECK(bool(ogb));
    CHECK(ogb->val == "res1");

    optional<Guard> ogc(in_place);   // default-constructs the contained value
    CHECK(bool(ogc));
    CHECK(ogc->val == "");

    oga.emplace("res1");                  // initialzes the contained value with "res1"
    CHECK(bool(oga));
    CHECK(oga->val == "res1");

    oga.emplace();                        // destroys the contained value and
                                          // default-constructs the new one
    CHECK(bool(oga));
    CHECK(oga->val == "");

    oga = nullopt;                        // OK: make disengaged the optional Guard
    CHECK(!(oga));
    //FAILS: ogb = {};                    // ERROR: Guard is not Moveable
}

void process () {}
void process (int) {}
void processNil () {}

TEST_CASE("example1")
{
    optional<int> oi;           // create disengaged object
    optional<int> oj = nullopt; // alternative syntax
    oi = oj;                    // assign disengaged object
    optional<int> ok = oj;      // ok is disengaged

    if (oi)
        CHECK(false);    // 'if oi is engaged...'

    if (!oi)
        CHECK(true);     // 'if oi is disengaged...'

    if (oi != nullopt)
        CHECK(false);    // 'if oi is engaged...'

    if (oi == nullopt)
        CHECK(true);     // 'if oi is disengaged...'

    CHECK(oi == ok);     // two disengaged optionals compare equal

    ///////////////////////////////////////////////////////////////////////////
    optional<int> ol{1}; // ol is engaged; its contained value is 1
    ok = 2;              // ok becomes engaged; its contained value is 2
    oj = ol;             // oj becomes engaged; its contained value is 1

    CHECK(oi != ol);     // disengaged != engaged
    CHECK(ok != ol);     // different contained values
    CHECK(oj == ol);     // same contained value
    CHECK(oi < ol);      // disengaged < engaged
    CHECK(ol < ok);      // less by contained value

    /////////////////////////////////////////////////////////////////////////////
    optional<int> om{1};   // om is engaged; its contained value is 1
    optional<int> on = om; // on is engaged; its contained value is 1
    om = 2;                // om is engaged; its contained value is 2
    CHECK(on != om);       // on still contains 3. They are not pointers

    /////////////////////////////////////////////////////////////////////////////
    int i = *ol;           // i obtains the value contained in ol
    CHECK(i == 1);
    *ol = 9;               // the object contained in ol becomes 9
    CHECK(*ol == 9);
    CHECK(ol == make_optional(9));

    ///////////////////////////////////
    int p = 1;
    optional<int> op = p;
    CHECK(*op == 1);
    p = 2;
    CHECK(*op == 1);  // value contained in op is separated from p

    ////////////////////////////////
    if (ol)
        process(*ol); // use contained value if present
    else
        process();    // proceed without contained value

    if (!om)
    processNil();
    else
    process(*om);

    /////////////////////////////////////////
    process(ol.value_or(0)); // use 0 if ol is disengaged

    ////////////////////////////////////////////
    ok = nullopt;   // if ok was engaged calls T's dtor
    oj = {};        // assigns a temporary disengaged optional
}

TEST_CASE("example_guard")
{
    const optional<int> c = 4;
    int i = *c;          // i becomes 4
    CHECK(i == 4);
    // FAILS: *c = i;    // ERROR: cannot assign to const int&
};

// There is no specialization for reference in standard (unlike Boost's implementation)
TEST_CASE("example_ref")
{
    int i = 1;
    int j = 2;

    // optional<int &> ora;     // disengaged optional reference to int
    optional<std::reference_wrapper<int>> ora;

    // optional<int &> orb = i; // contained reference refers to object i
    optional<std::reference_wrapper<int>> orb = std::ref(i);

    //*orb = 3;  // ERROR: use of deleted function `std::reference_wrapper::reference_wrapper(T&&)`
    ora = std::ref(j);
    ora = {j};
    ora = orb;
    ora.emplace(j); // OK: contained reference refers to object j
    ora.emplace(i); // OK: contained reference now refers to object i

    ora = nullopt;  // OK: ora becomes disengaged
}

template <typename T>
T getValue (optional<T> newVal = nullopt, optional<std::reference_wrapper<T>> storeHere = nullopt)
{
    T cached {};

    if (newVal) {
        cached = *newVal;

        if (storeHere) {
            *storeHere = *newVal; // LEGAL: assigning T to T
        }
    }

    return cached;
}

TEST_CASE("example_optional_arg")
{
    int iii = 0;
    iii = getValue<int>(iii, std::ref(iii));
    iii = getValue<int>(iii);
    iii = getValue<int>();

    {
        optional<Guard> grd1{in_place, "res1", 1};   // guard 1 initialized
        optional<Guard> grd2;

        grd2.emplace("res2", 2);  // guard 2 initialized
        grd1 = nullopt;           // guard 1 released
    }                             // guard 2 released (in dtor)
}

std::tuple<Date, Date, Date> getStartMidEnd ()
{
    return std::tuple<Date, Date, Date>{Date{1}, Date{2}, Date{3}};
}

void run (Date const &, Date const &, Date const &)
{}

TEST_CASE("example_date")
{
    optional<Date> start, mid, end; // Date doesn't have default ctor (no good default date)

    std::tie(start, mid, end) = getStartMidEnd();
    run(*start, *mid, *end);
};

optional<char> readNextChar ()
{
    return{};
}

void run (optional<std::string>)
{}

void run (std::complex<double>)
{}

template <class T>
void assign_norebind (optional<std::reference_wrapper<T>> & optref, T & obj)
{
    if (optref)
        *optref = obj;
    else
        optref.emplace(obj);
}

template <typename T>
void unused (T &&)
{}

TEST_CASE("example_conceptual_model")
{
    optional<int> oi = 0;
    optional<int> oj = 1;
    optional<int> ok = nullopt;

    oi = 1;
    oj = nullopt;
    ok = 0;

    unused(oi == nullopt);
    unused(oj == 0);
    unused(ok == 1);
}

TEST_CASE("example_rationale")
{
    if (optional<char> ch = readNextChar()) {
        unused(ch);
        // ...
    }

    //////////////////////////////////
    optional<int> opt1 = nullopt;
    optional<int> opt2 = {};

    opt1 = nullopt;
    opt2 = {};

    if (opt1 == nullopt) {}
    if (!opt2) {}
    if (opt2 == optional<int>{}) {}

    ////////////////////////////////

    run(nullopt);      // pick the second overload
    // FAILS: run({}); // ambiguous

    if (opt1 == nullopt) {} // fine
    // FAILS: if (opt2 == {}) {}   // ilegal

    ////////////////////////////////
    CHECK(optional<unsigned>{}  < optional<unsigned>{0});
    CHECK(optional<unsigned>{0} < optional<unsigned>{1});
    CHECK(!(optional<unsigned>{}  < optional<unsigned>{}) );
    CHECK(!(optional<unsigned>{1} < optional<unsigned>{1}));

    CHECK(optional<unsigned>{}  != optional<unsigned>{0});
    CHECK(optional<unsigned>{0} != optional<unsigned>{1});
    CHECK(optional<unsigned>{}  == optional<unsigned>{} );
    CHECK(optional<unsigned>{0} == optional<unsigned>{0});

    /////////////////////////////////
    optional<int> o;
    o = make_optional(1); // copy/move assignment
    o = 1;                     // assignment from T
    o.emplace(1);              // emplacement

    ////////////////////////////////////
    int isas = 0, i = 9;
    optional<std::reference_wrapper<int>> asas = std::ref(i);
    assign_norebind(asas, isas);

    /////////////////////////////////////
    ////optional<std::vector<int>> ov2 = {2, 3};
    ////CHECK(bool(ov2));
    ////CHECK((*ov2)[1] == 3);
    ////
    ////////////////////////////////
    ////std::vector<int> v = {1, 2, 4, 8};
    ////optional<std::vector<int>> ov = {1, 2, 4, 8};

    ////CHECK(v == *ov);
    ////
    ////ov = {1, 2, 4, 8};

    ////std::allocator<int> a;
    ////optional<std::vector<int>> ou { in_place, {1, 2, 4, 8}, a };

    ////CHECK(ou == ov);

    //////////////////////////////
    // inconvenient syntax:
    {

        optional<std::vector<int>> ov2{in_place, {2, 3}};

        CHECK(bool(ov2));
        CHECK((*ov2)[1] == 3);

        ////////////////////////////

        std::vector<int> v = {1, 2, 4, 8};
        optional<std::vector<int>> ov{in_place, {1, 2, 4, 8}};

        CHECK(v == *ov);

        ov.emplace({1, 2, 4, 8});
    /*
        std::allocator<int> a;
        optional<std::vector<int>> ou { in_place, {1, 2, 4, 8}, a };

        CHECK(ou == ov);
    */
    }

    /////////////////////////////////
    {
        typedef int T;
        optional<optional<T>> ot {in_place};
        optional<optional<T>> ou {in_place, nullopt};
        optional<optional<T>> ov {optional<T>{}};

        optional<int> oi;
        auto ooi = make_optional(oi);
        static_assert(std::is_same<optional<optional<int>>
            , decltype(ooi)>::value
            , "");
    }
}

bool fun (std::string , optional<int> oi = nullopt)
{
    return bool(oi);
}

TEST_CASE("example_converting_ctor")
{
    CHECK(true == fun("dog", 2));
    CHECK(false == fun("dog"));
    CHECK(false == fun("dog", nullopt)); // just to be explicit
}

TEST_CASE("bad_comparison")
{
    optional<int> oi, oj;
    int i;
    bool b = (oi == oj);
    b = (oi >= i);
    b = (oi == i);
    unused(b);
};

TEST_CASE("value_or")
{
    optional<int> oi = 1;
    int i = oi.value_or(0);
    CHECK(i == 1);

    oi = nullopt;
    CHECK(oi.value_or(3) == 3);

    optional<std::string> os{"AAA"};
    CHECK(os.value_or("BBB") == "AAA");
    os = {};
    CHECK(os.value_or("BBB") == "BBB");
}

TEST_CASE("reset")
{
    optional<int> oi {1};
    oi.reset();
    CHECK(!oi);

    int i = 1;
    optional<std::reference_wrapper<const int>> oir {i};
    oir.reset();
    CHECK(!oir);
};

TEST_CASE("mixed_order")
{
    optional<int> oN {nullopt};
    optional<int> o0 {0};
    optional<int> o1 {1};

    CHECK( (oN <   0));
    CHECK( (oN <   1));
    CHECK(!(o0 <   0));
    CHECK( (o0 <   1));
    CHECK(!(o1 <   0));
    CHECK(!(o1 <   1));

    CHECK(!(oN >=  0));
    CHECK(!(oN >=  1));
    CHECK( (o0 >=  0));
    CHECK(!(o0 >=  1));
    CHECK( (o1 >=  0));
    CHECK( (o1 >=  1));

    CHECK(!(oN >   0));
    CHECK(!(oN >   1));
    CHECK(!(o0 >   0));
    CHECK(!(o0 >   1));
    CHECK( (o1 >   0));
    CHECK(!(o1 >   1));

    CHECK( (oN <=  0));
    CHECK( (oN <=  1));
    CHECK( (o0 <=  0));
    CHECK( (o0 <=  1));
    CHECK(!(o1 <=  0));
    CHECK( (o1 <=  1));

    CHECK( (0 >  oN));
    CHECK( (1 >  oN));
    CHECK(!(0 >  o0));
    CHECK( (1 >  o0));
    CHECK(!(0 >  o1));
    CHECK(!(1 >  o1));

    CHECK(!(0 <= oN));
    CHECK(!(1 <= oN));
    CHECK( (0 <= o0));
    CHECK(!(1 <= o0));
    CHECK( (0 <= o1));
    CHECK( (1 <= o1));

    CHECK(!(0 <  oN));
    CHECK(!(1 <  oN));
    CHECK(!(0 <  o0));
    CHECK(!(1 <  o0));
    CHECK( (0 <  o1));
    CHECK(!(1 <  o1));

    CHECK( (0 >= oN));
    CHECK( (1 >= oN));
    CHECK( (0 >= o0));
    CHECK( (1 >= o0));
    CHECK(!(0 >= o1));
    CHECK( (1 >= o1));
};

struct BadRelops
{
    int i;
};

constexpr bool operator < (BadRelops a, BadRelops b)
{
    return a.i < b.i;
}

constexpr bool operator > (BadRelops a, BadRelops b)
{
    return a.i > b.i; // intentional error!
}

TEST_CASE("bad_relops")
{
    BadRelops a{1}, b{2};
    CHECK(a < b);
    CHECK_FALSE(a > b);

    optional<BadRelops> oa = a, ob = b;
    CHECK(oa < ob);
    CHECK_FALSE(oa > ob);

    CHECK(oa < b);
    CHECK_FALSE(oa > b);

    optional<std::reference_wrapper<BadRelops>> ra = std::ref(a)
        , rb = std::ref(b);
    CHECK(ra < rb);
    CHECK_FALSE((ra > rb));

    CHECK(ra < std::ref(b));
    CHECK_FALSE(ra > std::ref(b));
}

TEST_CASE("mixed_equality")
{
    CHECK(make_optional(0) == 0);
    CHECK(make_optional(1) == 1);
    CHECK(make_optional(0) != 1);
    CHECK(make_optional(1) != 0);

    optional<int> oN {nullopt};
    optional<int> o0 {0};
    optional<int> o1 {1};

    CHECK(o0 ==  0);
    CHECK( 0 == o0);
    CHECK(o1 ==  1);
    CHECK( 1 == o1);
    CHECK(o1 !=  0);
    CHECK( 0 != o1);
    CHECK(o0 !=  1);
    CHECK( 1 != o0);

    CHECK( 1 != oN);
    CHECK( 0 != oN);
    CHECK(oN !=  1);
    CHECK(oN !=  0);
    CHECK(!( 1 == oN));
    CHECK(!( 0 == oN));
    CHECK(!(oN ==  1));
    CHECK(!(oN ==  0));

    std::string cat{"cat"}, dog{"dog"};
    optional<std::string> oNil{}, oDog{"dog"}, oCat{"cat"};

    CHECK(oCat ==  cat);
    CHECK( cat == oCat);
    CHECK(oDog ==  dog);
    CHECK( dog == oDog);
    CHECK(oDog !=  cat);
    CHECK( cat != oDog);
    CHECK(oCat !=  dog);
    CHECK( dog != oCat);

    CHECK( dog != oNil);
    CHECK( cat != oNil);
    CHECK(oNil !=  dog);
    CHECK(oNil !=  cat);
    CHECK(!( dog == oNil));
    CHECK(!( cat == oNil));
    CHECK(!(oNil ==  dog));
    CHECK(!(oNil ==  cat));
};

TEST_CASE("const_propagation")
{
    optional<int> mmi{0};
    static_assert(std::is_same<decltype(*mmi), int&>::value, "WTF");

    const optional<int> cmi{0};
    static_assert(std::is_same<decltype(*cmi), const int&>::value, "WTF");

    optional<const int> mci{0};
    static_assert(std::is_same<decltype(*mci), const int&>::value, "WTF");

    optional<const int> cci{0};
    static_assert(std::is_same<decltype(*cci), const int&>::value, "WTF");
}

// static_assert(std::is_base_of<std::logic_error
//     , std::bad_optional_access>::value
//     , "");

TEST_CASE("safe_value")
{
    try {
        optional<int> ovN{}, ov1{1};

        int & r1 = ov1.value();
        CHECK(r1 == 1);

        try {
            ovN.value();
            CHECK(false);
        } catch (bad_optional_access const &) {
        }

        { // ref variant
            int i1 = 1;
            optional<std::reference_wrapper<int>> orN{}, or1{i1};

            int & r2 = or1.value();
            CHECK(r2 == 1);

            try {
                orN.value();
                CHECK(false);
            } catch (bad_optional_access const &) {
            }
        }
    } catch (...) {
        CHECK(false);
    }
}

TEST_CASE("optional_ref")
{
    // FAILS: optional<int&&> orr;
    // FAILS: optional<nullopt_t&> on;
    int i = 8;
    optional<std::reference_wrapper<int>> ori;
    CHECK(!ori);
    ori.emplace(i);
    CHECK(bool(ori));
    CHECK(*ori == 8);
    CHECK((& ori->get() == & i));
    (*ori)++;
    CHECK(i == 9);

    // FAILS: int& ir = ori.value_or(i);
    int ii = ori.value_or(i);
    CHECK(ii == 9);
    ii = 7;
    CHECK(*ori == 9);

    int j = 22;
    auto && oj = make_optional(std::ref(j));
    (*oj)++;

//     CHECK((& *oj == & j));
    CHECK(j == 23);
}

TEST_CASE("optional_ref_const_propagation")
{
    int i = 9;
    const optional<std::reference_wrapper<int>> mi = std::ref(i);
    int & r = *mi;
    optional<std::reference_wrapper<int const>> ci = std::cref(i);

    static_assert(std::is_same<decltype(*mi), std::reference_wrapper<int> const &>::value, "WTF");
    static_assert(std::is_same<decltype(*ci), std::reference_wrapper<int const> &>::value, "WTF");

    unused(r);
};

TEST_CASE("optional_ref_assign")
{
    int i = 9;
    optional<std::reference_wrapper<int>> ori = std::ref(i);

    int j = 1;
    ori = optional<std::reference_wrapper<int>>{j};
    ori = {j};
    // FAILS: ori = j;

    optional<std::reference_wrapper<int>> orx = ori;
    ori = orx;

    optional<std::reference_wrapper<int>> orj = std::ref(j);

    CHECK(ori);
    CHECK(*ori == 1);
    CHECK(ori == orj);
    CHECK(i == 9);

    ++*ori;
    CHECK(*ori == 2);
    CHECK(2 == *ori);
    CHECK(*ori != 3);

    CHECK(ori == orj);
    CHECK(j == 2);
    CHECK(i == 9);

    ori = {};
    CHECK(!ori);
    CHECK(ori != orj);
    CHECK(j == 2);
    CHECK(i == 9);
}

TEST_CASE("optional_ref_swap")
{
    int i = 0;
    int j = 1;
    optional<std::reference_wrapper<int>> oi = std::ref(i);
    optional<std::reference_wrapper<int>> oj = std::ref(j);

    CHECK(& oi->get() == & i);
    CHECK(& oj->get() == & j);

    using std::swap;
    swap(oi, oj);
    CHECK(& oi->get() == & j);
    CHECK(& oj->get() == & i);
};

TEST_CASE("optional_initialization")
{
    using std::string;
    string s = "STR";

    optional<string> os{s};
    optional<string> ot = s;
    optional<string> ou{"STR"};
    optional<string> ov = string{"STR"};

}

#include <unordered_set>

TEST_CASE("optional_hashing")
{
    using std::string;

    std::hash<int> hi;
    std::hash<optional<int>> hoi;
    std::hash<string> hs;
    std::hash<optional<string>> hos;

    CHECK(hi(0) == hoi(optional<int>{0}));
    CHECK(hi(1) == hoi(optional<int>{1}));
    CHECK(hi(3198) == hoi(optional<int>{3198}));

    CHECK(hs("") == hos(optional<string>{""}));
    CHECK(hs("0") == hos(optional<string>{"0"}));
    CHECK(hs("Qa1#") == hos(optional<string>{"Qa1#"}));

    std::unordered_set<optional<string>> set;
    CHECK(set.find({"Qa1#"}) == set.end());

    set.insert({"0"});
    CHECK(set.find({"Qa1#"}) == set.end());

    set.insert({"Qa1#"});
    CHECK(set.find({"Qa1#"}) != set.end());
}

// optional_ref_emulation
template <typename T>
struct generic
{
  typedef T type;
};

template <typename U>
struct generic<U&>
{
  typedef std::reference_wrapper<U> type;
};

template <typename T>
using Generic = typename generic<T>::type;

template <typename X>
bool generic_fun()
{
  optional<Generic<X>> op;
  return bool(op);
}

TEST_CASE("optional_ref_emulation")
{
    optional<Generic<int>> oi = 1;
    CHECK(*oi == 1);

    int i = 8;
    int j = 4;
    optional<Generic<int&>> ori {i};
    CHECK(*ori == 8);
    CHECK((void*)&*ori != (void*)&i); // !DIFFERENT THAN optional<T&>

    *ori = j;
    CHECK(*ori == 4);
};

# if OPTIONAL_HAS_THIS_RVALUE_REFS == 1
TEST_CASE("moved_on_value_or")
{
    optional<Oracle> oo{in_place};

    CHECK(oo);
    CHECK(oo->s == sDefaultConstructed);

    Oracle o = std::move(oo).value_or( Oracle{OracleVal{}} );
    CHECK(oo);
    CHECK(oo->s == sMovedFrom);
    CHECK(o.s == sMoveConstructed);

    optional<MoveAware<int>> om {in_place, 1};
    CHECK(om);
    CHECK(om->moved == false);

    /*MoveAware<int> m =*/ std::move(om).value_or( MoveAware<int>{1} );
    CHECK(om);
    CHECK(om->moved == true);

#   if OPTIONAL_HAS_MOVE_ACCESSORS == 1
    {
        Date d = optional<Date>{in_place, 1}.value();
        CHECK(d.i); // to silence compiler warning

        Date d2 = *optional<Date>{in_place, 1};
        CHECK(d2.i); // to silence compiler warning
    }
#   endif
}
#endif

namespace std {

namespace details {
    template <typename T>
    struct hash_helper
    {
        std::size_t operator () (std::reference_wrapper<T> const & ref) const noexcept
        {
            return std::hash<T>{}(ref.get());
        }
    };

    template <typename T>
    struct equal_to_helper
    {
        constexpr bool operator () (std::reference_wrapper<T> const & a
            , std::reference_wrapper<T> const & b) const
        {
            return a.get() == b.get();
        }
    };
}

template <>
struct hash<std::reference_wrapper<int>> : details::hash_helper<int> {};

template <>
struct hash<std::reference_wrapper<std::string>> : details::hash_helper<std::string> {};

template <>
struct equal_to<std::reference_wrapper<int>> : details::equal_to_helper<int> {};

template <>
struct equal_to<std::reference_wrapper<std::string>> : details::equal_to_helper<std::string> {};

} // namespace std

struct equal_to_opt_string_ref
{
    bool operator () (optional<std::reference_wrapper<std::string>> const & a
        , optional<std::reference_wrapper<std::string>> const & b) const
    {
        if (a != nullopt && b != nullopt)
            return a->get() == b->get();

        if (a == nullopt && b == nullopt)
            return true;

        return false;
    }
};

struct hash_opt_string_ref
{
    std::size_t operator () (optional<std::reference_wrapper<std::string>> const & x) const noexcept
    {
        if (x == nullopt)
            return 0;
        return std::hash<std::string>{}(x->get());
    }
};

// FIXME
// TEST_CASE("optional_ref_hashing")
// {
//     using std::string;
//     using int_ref_t = std::reference_wrapper<int>;
//     using string_ref_t = std::reference_wrapper<string>;
//
//     std::hash<int> hi;
//     std::hash<optional<int_ref_t>> hoi;
//     std::hash<string> hs;
//     std::hash<optional<string_ref_t>> hos;
//
//     int i0 = 0;
//     int i1 = 1;
//     CHECK(hi(0) == hoi(optional<int_ref_t>{i0}));
//     CHECK(hi(1) == hoi(optional<int_ref_t>{i1}));
//
//     string s{""};
//     string s0{"0"};
//     string sCAT{"CAT"};
//     CHECK(hs("") == hos(optional<string_ref_t>{s}));
//     CHECK(hs("0") == hos(optional<string_ref_t>{s0}));
//     CHECK(hs("CAT") == hos(optional<string_ref_t>{sCAT}));
//
//     std::unordered_set<optional<string_ref_t>
//         , hash_opt_string_ref
//         , equal_to_opt_string_ref> set;
//
//     CHECK(set.find({sCAT}) == set.end());
//
//     set.insert({s0});
//     CHECK(set.find({sCAT}) == set.end());
//
//     set.insert({sCAT});
//     CHECK(set.find({sCAT}) != set.end());
// };

struct Combined
{
    int m = 0;
    int n = 1;

    constexpr Combined () : m{5}, n{6} {}
    constexpr Combined (int m, int n) : m{m}, n{n} {}
};

struct Nasty
{
    int m = 0;
    int n = 1;

    constexpr Nasty () : m{5}, n{6} {}
    constexpr Nasty (int m, int n) : m{m}, n{n} {}

    int operator & () { return n; }
    int operator & () const { return n; }
};

TEST_CASE("arrow_operator")
{
    optional<Combined> oc1{in_place, 1, 2};
    CHECK(oc1);
    CHECK(oc1->m == 1);
    CHECK(oc1->n == 2);

    optional<Nasty> on{in_place, 1, 2};
    CHECK(on);
    CHECK(on->m == 1);
    CHECK(on->n == 2);
}

TEST_CASE("arrow_wit_optional_ref")
{
    Combined c{1, 2};
    optional<std::reference_wrapper<Combined>> oc = std::ref(c);

    CHECK(oc);
    CHECK(oc->get().m == 1);
    CHECK(oc->get().n == 2);

    Nasty n{1, 2};
    Nasty m{3, 4};
    Nasty p{5, 6};

    optional<std::reference_wrapper<Nasty>> on{n};
    CHECK(on);
    CHECK(on->get().m == 1);
    CHECK(on->get().n == 2);

    on = {m};
    CHECK(on);
    CHECK(on->get().m == 3);
    CHECK(on->get().n == 4);

    on.emplace(p);
    CHECK(on);
    CHECK(on->get().m == 5);
    CHECK(on->get().n == 6);

    optional<std::reference_wrapper<Nasty>> om{in_place, n};
    CHECK(om);
    CHECK(om->get().m == 1);
    CHECK(om->get().n == 2);
}

TEST_CASE("no_dangling_reference_in_value")
{
    // this mostly tests compiler warnings
    optional<int> oi {2};
    unused (oi.value());
    const optional<int> coi {3};
    unused (coi.value());
};

struct CountedObject
{
    static int _counter;
    bool _throw;
    CountedObject (bool b) : _throw(b)
    {
        ++_counter;
    }

    CountedObject (CountedObject const & rhs)
        : _throw(rhs._throw)
    {
        if (_throw) throw int();
    }

    ~CountedObject ()
    {
        --_counter;
    }
};

int CountedObject::_counter = 0;

TEST_CASE("exception_safety")
{
    try {
        optional<CountedObject> oo(in_place, true); // throw
        optional<CountedObject> o1(oo);
    } catch(...) {
        //
    }
    CHECK(CountedObject::_counter == 0);

    try {
        optional<CountedObject> oo(in_place, true); // throw
        optional<CountedObject> o1(std::move(oo));  // now move
    } catch(...) {
        //
    }

    CHECK(CountedObject::_counter == 0);
};

TEST_CASE("nested_optional")
{
    optional<optional<optional<int>>> o1 {nullopt};
    CHECK(!o1);

    optional<optional<optional<int>>> o2 {in_place, nullopt};
    CHECK(o2);
    CHECK(!*o2);

    optional<optional<optional<int>>> o3 (in_place, in_place, nullopt);
    CHECK(o3);
    CHECK(*o3);
    CHECK(!**o3);
};

//// constexpr tests

// these 4 classes have different noexcept signatures in move operations
struct NothrowBoth {
    NothrowBoth (NothrowBoth &&) noexcept(true) {}
    void operator = (NothrowBoth &&) noexcept(true) {}
};

struct NothrowCtor
{
    NothrowCtor (NothrowCtor &&) noexcept(true) {}
    void operator = (NothrowCtor &&) noexcept(false) {}
};

struct NothrowAssign
{
    NothrowAssign (NothrowAssign &&) noexcept(false) {}
    void operator = (NothrowAssign &&) noexcept(true) {}
};

struct NothrowNone
{
    NothrowNone (NothrowNone &&) noexcept(false) {}
    void operator = (NothrowNone &&) noexcept(false) {}
};

// FIXME
// TEST_CASE("test_noexcept")
// {
//     {
//         optional<NothrowBoth> b1, b2;
//         static_assert(noexcept(optional<NothrowBoth>{constexpr_move(b1)}), "bad noexcept!");
//         static_assert(noexcept(b1 = constexpr_move(b2)), "bad noexcept!");
//     }
//
//     {
//         optional<NothrowCtor> c1, c2;
//         static_assert(noexcept(optional<NothrowCtor>{constexpr_move(c1)}), "bad noexcept!");
//         static_assert(!noexcept(c1 = constexpr_move(c2)), "bad noexcept!");
//     }
//
//     {
//         optional<NothrowAssign> a1, a2;
//         static_assert(!noexcept(optional<NothrowAssign>{constexpr_move(a1)}), "bad noexcept!");
//         static_assert(!noexcept(a1 = constexpr_move(a2)), "bad noexcept!");
//     }
//
//     {
//         optional<NothrowNone> n1, n2;
//         static_assert(!noexcept(optional<NothrowNone>{constexpr_move(n1)}), "bad noexcept!");
//         static_assert(!noexcept(n1 = constexpr_move(n2)), "bad noexcept!");
//     }
// }

TEST_CASE("constexpr_test_disengaged")
{
    constexpr optional<int> g0{};
    constexpr optional<int> g1{nullopt};
    static_assert( !g0, "initialized!" );
    static_assert( !g1, "initialized!" );

    static_assert( bool(g1) == bool(g0), "ne!" );

    static_assert( g1 == g0, "ne!" );
    static_assert( !(g1 != g0), "ne!" );
    static_assert( g1 >= g0, "ne!" );
    static_assert( !(g1 > g0), "ne!" );
    static_assert( g1 <= g0, "ne!" );
    static_assert( !(g1 <g0), "ne!" );

    static_assert( g1 == nullopt, "!" );
    static_assert( !(g1 != nullopt), "!" );
    static_assert( g1 <= nullopt, "!" );
    static_assert( !(g1 < nullopt), "!" );
    static_assert( g1 >= nullopt, "!" );
    static_assert( !(g1 > nullopt), "!" );

    static_assert(  (nullopt == g0), "!" );
    static_assert( !(nullopt != g0), "!" );
    static_assert(  (nullopt >= g0), "!" );
    static_assert( !(nullopt >  g0), "!" );
    static_assert(  (nullopt <= g0), "!" );
    static_assert( !(nullopt <  g0), "!" );

    static_assert(  (g1 != optional<int>(1)), "!" );
    static_assert( !(g1 == optional<int>(1)), "!" );
    static_assert(  (g1 <  optional<int>(1)), "!" );
    static_assert(  (g1 <= optional<int>(1)), "!" );
    static_assert( !(g1 >  optional<int>(1)), "!" );
    static_assert( !(g1 >  optional<int>(1)), "!" );
}

constexpr optional<int> g0{};
constexpr optional<int> g2{2};
static_assert( g2, "not initialized!" );
static_assert( *g2 == 2, "not 2!" );
static_assert( g2 == optional<int>(2), "not 2!" );
static_assert( g2 != g0, "eq!" );

# if OPTIONAL_HAS_MOVE_ACCESSORS == 1
static_assert( *optional<int>{3} == 3, "WTF!" );
static_assert( optional<int>{3}.value() == 3, "WTF!" );
static_assert( optional<int>{3}.value_or(1) == 3, "WTF!" );
static_assert( optional<int>{}.value_or(4) == 4, "WTF!" );
# endif

constexpr optional<Combined> gc0{in_place};
static_assert(gc0->n == 6, "WTF!");

// constexpr int gci = 1;
// constexpr optional<std::reference_wrapper<int const>> gorci = gci;
// constexpr optional<std::reference_wrapper<int const>> gorcn{};
//
// static_assert(gorcn <  gorci, "WTF");
// static_assert(gorcn <= gorci, "WTF");
// static_assert(gorci == gorci, "WTF");
// static_assert(*gorci == 1, "WTF");
// static_assert(gorci == gci, "WTF");

// namespace constexpr_optional_ref_and_arrow
// {
//   using namespace stx;
//   constexpr Combined c{1, 2};
//   constexpr optional<Combined const&> oc = c;
//   static_assert(oc, "WTF!");
//   static_assert(oc->m == 1, "WTF!");
//   static_assert(oc->n == 2, "WTF!");
// }

#if OPTIONAL_HAS_CONSTEXPR_INIT_LIST

namespace InitList {

    struct ConstInitLister
    {
        template <typename T>
        constexpr ConstInitLister (std::initializer_list<T> il)
            : len (il.size())
        {}
        size_t len;
    };

    constexpr ConstInitLister CIL {2, 3, 4};
    static_assert(CIL.len == 3, "WTF!");

    constexpr optional<ConstInitLister> oil {in_place, {4, 5, 6, 7}};
    static_assert(oil, "WTF!");
    static_assert(oil->len == 4, "WTF!");
}

#endif // OPTIONAL_HAS_CONSTEXPR_INIT_LIST

// end constexpr tests

struct VEC
{
    std::vector<int> v;

    template <typename... X>
    VEC (X &&... x) : v(std::forward<X>(x)...) {}

    template <typename U, typename... X>
    VEC(std::initializer_list<U> il, X &&... x)
        : v(il, std::forward<X>(x)...)
    {}
};

TEST_CASE("") {
    optional<int> oi = 1;

    CHECK(bool(oi));
    oi.operator = ({});
    CHECK(!oi);

    VEC v = {5, 6};

#ifdef OPTIONAL_HAS_THIS_RVALUE_REFS
    if (OPTIONAL_HAS_THIS_RVALUE_REFS)
        MESSAGE("Optional has rvalue references for *this");
    else
        MESSAGE("Optional doesn't have rvalue references for *this");
#endif

#ifdef OPTIONAL_HAS_CONSTEXPR_INIT_LIST
    if (OPTIONAL_HAS_CONSTEXPR_INIT_LIST)
        MESSAGE("Optional has constexpr initializer_list");
    else
        MESSAGE("Optional doesn't have constexpr initializer_list");
#endif

#ifdef OPTIONAL_HAS_MOVE_ACCESSORS
    if (OPTIONAL_HAS_MOVE_ACCESSORS)
        MESSAGE("Optional has constexpr move accessors");
    else
        MESSAGE("Optional doesn't have constexpr move accessors");
#endif
}
