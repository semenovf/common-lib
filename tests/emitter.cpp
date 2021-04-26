////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 Vladislav Trifochkin
//
// This file is part of [pfs-modulus](https://github.com/semenovf/pfs-modulus) library.
//
// Changelog:
//      2019.12.19 Initial version (inhereted from https://github.com/semenovf/pfs)
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define ANKERL_NANOBENCH_IMPLEMENT
#include "doctest.h"
#include "nanobench.h"
#include "pfs/emitter.hpp"
#include <string>

namespace t0 {
    std::string check;

    void f ()
    {
        //std::cout << "t0::f()\n";
        check.push_back('a');
    }

    void (*fptr) () = f;

    void f (int x)
    {
        //std::cout << "t0::f(" << x << ")\n";
        check.push_back('b');
    }

    void f (int x, std::string const & s)
    {
        //std::cout << "t0::f(" << x << ", \"" << s << "\")\n";
        check.push_back('c');
    }

    void benchmark (int x, std::string const & s) {}

    class A {
    public:
        void f ()
        {
            //std::cout << "t0::A::f()\n";
            check.push_back('A');
        }

        void f (int x)
        {
            //std::cout << "t0::A::f(" << x << ")\n";
            check.push_back('B');
        }

        void f (int x, std::string const & s)
        {
            //std::cout << "t0::A::f(" << x << ", \"" << s << "\")\n";
            check.push_back('C');
        }

        void benchmark (int x, std::string const & s) {}
    };

    auto lambda0 = [] {
        check.push_back('x');
    };

    auto lambda1 = [] (int) {
        check.push_back('y');
    };

    auto lambda2 = [] (int, std::string const &) {
        check.push_back('z');
    };

} // namespace t0

TEST_CASE("Basic emitter / detector") {
    t0::check.clear();

    pfs::emitter<> em0;
    pfs::emitter<int> em1;
    pfs::emitter<int, std::string const &> em2;

    t0::A a;

    em0.connect(t0::fptr);
    em0.connect(static_cast<void(*)()>(t0::f));
    em0.connect(std::function<void()>{static_cast<void(*)()>(& t0::f)});
    em0.connect(a, & t0::A::f);
    em0.connect(t0::lambda0);
    em0.connect([] { std::cout << "anonymous lambda call()\n"; });

    em1.connect(static_cast<void(*)(int)>(t0::f));
    em1.connect(a, & t0::A::f);
    em1.connect(t0::lambda1);
    em1.connect([] (int x) {
        std::cout << "anonymous lambda call(" << x << ")\n";
    });

    em2.connect(static_cast<void(*)(int, std::string const &)>(t0::f));
    em2.connect(a, & t0::A::f);
    em2.connect(t0::lambda2);
    em2.connect([] (int x, std::string const & s) {
        std::cout << "anonymous lambda call(" << x << ", \"" << s << "\")\n";
    });

    em0();
    em1(42);
    em2(42, "Viva la PFS");

    CHECK_EQ(t0::check, "aaaAxbBycCz");
}

TEST_CASE("Emitter iterator and disconnect") {
    pfs::emitter<int> em;
    t0::A a;

    auto it0 = em.connect(static_cast<void(*)(int)>(t0::f));
    auto it1 = em.connect(a, & t0::A::f);
    auto it2 = em.connect(t0::lambda1);

    t0::check.clear();
    em(42);
    CHECK_EQ(t0::check, "bBy");

    t0::check.clear();
    (*it0)(42);
    (*it1)(42);
    (*it2)(42);
    CHECK_EQ(t0::check, "bBy");

    t0::check.clear();
    em.disconnect(it1);
    em(42);
    CHECK_EQ(t0::check, "by");

    em.disconnect_all();
    em(42);
    t0::check.clear();
    CHECK_EQ(t0::check, "");
}

TEST_CASE("benchmark") {
    ankerl::nanobench::Bench().run("emitter", [] {
        t0::A a;
        pfs::emitter<int, std::string const &> em;
        em.connect(a, & t0::A::benchmark);

        for (int i = 0, count = std::numeric_limits<uint16_t>::max(); i < count; i++) {
            em(42, "Viva la PFS");
        }
        //ankerl::nanobench::doNotOptimizeAway(d);
    });

    ankerl::nanobench::Bench().run("direct", [&] {
        t0::A a;
        for (int i = 0, count = std::numeric_limits<uint16_t>::max(); i < count; i++) {
            a.benchmark(42, "Viva la PFS");
        }
    });
}

#if __COMMENT__

////////////////////////////////////////////////////////////////////////////////
// Direct signals / slots
////////////////////////////////////////////////////////////////////////////////
namespace t0 {

using sigslot = pfs::sigslot<>;

class A : public sigslot::slot_holder
{
public:
    int counter = 0;

public:
    void slot () { counter++; }
    void slot (int) { counter++; }
    void slot (int, double) { counter++; }
    void slot (int, double, std::string const &) { counter++; }
};

} // namespace t0

TEST_CASE("Direct signals / slots") {

    using t0::A;
    using t0::sigslot;

    A a;
    sigslot::signal<> sig0;
    sigslot::signal<int> sig1;
    sigslot::signal<int, double> sig2;
    sigslot::signal<int, double, std::string const &> sig3;

    sig0.connect(& a, static_cast<void (A::*)()>(& A::slot));
    sig1.connect(& a, static_cast<void (A::*)(int)>(& A::slot));
    sig2.connect(& a, static_cast<void (A::*)(int, double)>(& A::slot));
    sig3.connect(& a, static_cast<void (A::*)(int, double, std::string const &)>(& A::slot));

    sig0();
    sig1(42);
    sig2(42, 3.14f);
    sig3(42, 3.14f, "hello");

    CHECK(a.counter == 4);

    sigslot::emit_signal(sig0);
    sigslot::emit_signal(sig1, 42);
    sigslot::emit_signal(sig2, 42, 3.14f);
    sigslot::emit_signal(sig3, 42, 3.14f, "hello");

    CHECK(a.counter == 8);
}

////////////////////////////////////////////////////////////////////////////////
// Queued signals / slots
////////////////////////////////////////////////////////////////////////////////
namespace t1 {

using active_queue = pfs::active_queue<>;
using sigslot = pfs::sigslot<active_queue>;

class B : public sigslot::queued_slot_holder
{
public:
    int counter = 0;

public:
    void slot () { counter++; }
    void slot (int) { counter++; }
    void slot (int, double) { counter++; }
    void slot (int, double, std::string const &) { counter++; }
};

} // namespace t1

TEST_CASE("Queued signals / slots") {
    using t1::B;
    using t1::sigslot;

    B b;
    sigslot::signal<> sig0;
    sigslot::signal<int> sig1;
    sigslot::signal<int, double> sig2;
    sigslot::signal<int, double, std::string const &> sig3;

    sig0.connect(& b, static_cast<void (B::*)()>(& B::slot));
    sig1.connect(& b, static_cast<void (B::*)(int)>(& B::slot));
    sig2.connect(& b, static_cast<void (B::*)(int, double)>(& B::slot));
    sig3.connect(& b, static_cast<void (B::*)(int, double, std::string const &)>(& B::slot));

    sig0();
    sig1(42);
    sig2(42, 3.14f);
    sig3(42, 3.14f, "hello");

    CHECK(b.callback_queue().count() == 4);
    CHECK(b.counter == 0);

    b.callback_queue().call_all();

    CHECK(b.callback_queue().count() == 0);
    CHECK(b.counter == 4);

    sigslot::emit_signal(sig0);
    sigslot::emit_signal(sig1, 42);
    sigslot::emit_signal(sig2, 42, 3.14f);
    sigslot::emit_signal(sig3, 42, 3.14f, "hello");

    CHECK(b.callback_queue().count() == 4);
    CHECK(b.counter == 4);

    b.callback_queue().call_all();

    CHECK(b.callback_queue().count() == 0);

    CHECK(b.counter == 8);
}

#endif // __COMMENT__
