////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Vladislav Trifochkin
//
// This file is part of [common-lib](https://github.com/semenovf/pfs-modulus) library.
//
// Changelog:
//      2021.04.25 Initial version (moved from https://github.com/semenovf/pfs-modulus)
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define ANKERL_NANOBENCH_IMPLEMENT
#include "doctest.h"
#include "nanobench.h"
#include "pfs/emitter.hpp"
#include "pfs/function_queue.hpp"
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

namespace t1 {
    int counter = 4;

    void f0 (bool)
    {
        std::cout << "Hello from regular function\n";
        counter--;
    }

    void f (bool)
    {
        std::cout << "Viva la Function Queue from regular function\n";
        counter--;
    }

    class A {
    public:
        void f (bool)
        {
            std::cout << "Viva la Function Queue from member function\n";
            counter--;
        }
    };

    auto lambda = [] (bool) {
        std::cout << "Viva la Function Queue from lambda\n";
        counter--;
    };
}

TEST_CASE("Emitter with function_queue") {
    pfs::function_queue<> q;
    pfs::emitter<bool> em;
    t1::A a;

    em.connect(t1::f0);
    em.connect(q, t1::f);
    em.connect(q, t1::lambda);
    em.connect(q, a, & t1::A::f);

    // Nothing happened yet, but pushed into queue
    em(true);

    q.call_all();

    CHECK_EQ(t1::counter, 0);
}

TEST_CASE("benchmark") {
    ankerl::nanobench::Bench().run("emitter ST", [] {
        t0::A a;
        pfs::emitter<int, std::string const &> em;
        em.connect(a, & t0::A::benchmark);

        for (int i = 0, count = std::numeric_limits<uint16_t>::max(); i < count; i++) {
            em(42, "Viva la PFS");
        }
    });

    ankerl::nanobench::Bench().run("emitter MT", [] {
        t0::A a;
        pfs::emitter_mt<int, std::string const &> em;
        em.connect(a, & t0::A::benchmark);

        for (int i = 0, count = std::numeric_limits<uint16_t>::max(); i < count; i++) {
            em(42, "Viva la PFS");
        }
    });

    ankerl::nanobench::Bench().run("direct", [] {
        t0::A a;

        for (int i = 0, count = std::numeric_limits<uint16_t>::max(); i < count; i++) {
            a.benchmark(42, "Viva la PFS");
        }
    });
}

