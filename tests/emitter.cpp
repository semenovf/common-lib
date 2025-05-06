////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2021.04.25 Initial version (moved from https://github.com/semenovf/pfs-modulus)
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define ANKERL_NANOBENCH_IMPLEMENT
#define PFS__TEST_ENABLED
#include "doctest.h"
#include "nanobench.h"
#include "pfs/emitter.hpp"
#include "pfs/function_queue.hpp"
#include <functional>
#include <string>
#include <thread>

namespace t0 {
    std::string check;

    void f ()
    {
        //std::cout << "t0::f()\n";
        check.push_back('a');
    }

    void (*fptr) () = f;

    void f (int /*x*/)
    {
        //std::cout << "t0::f(" << x << ")\n";
        check.push_back('b');
    }

    void f (int /*x*/, std::string const & s)
    {
        //std::cout << "t0::f(" << x << ", \"" << s << "\")\n";
        check.push_back('c');
    }

    void benchmark (int /*x*/, std::string const & s) {}

    class A {
    public:
        void f ()
        {
            //std::cout << "t0::A::f()\n";
            check.push_back('A');
        }

        void f (int /*x*/)
        {
            //std::cout << "t0::A::f(" << x << ")\n";
            check.push_back('B');
        }

        void f (int /*x*/, std::string const & /*s*/)
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

namespace t2 {
    class A {
    public:
        pfs::emitter<bool> em;
    };

    class B {
    public:
        pfs::emitter<bool> em;

    public:
        void f (bool b)
        {
            em(b);
        }
    };
}

TEST_CASE("Call emitter from detector") {
    {
        pfs::emitter<bool> outer_em;
        pfs::emitter<bool> inner_em;

        inner_em.connect([] (bool b) {
            CHECK(b);
        });

        outer_em.connect([& inner_em] (bool b) {
            inner_em(b);
        });

        outer_em(true);
    }

    {
        t2::A a;
        t2::B b;

        a.em.connect([& b] (bool) {
            b.f(true);
        });

        b.em.connect([] (bool v) {
            CHECK(v);
        });

        a.em(true);
    }
}

namespace t3 {

class A
{
    int _counter = 0;

public:
    pfs::emitter<int> increment;

public:
    A ()
    {
        increment.connect([this] (int inc) {
            _counter += inc;
        });
    }

    int counter () const noexcept
    {
        return _counter;
    }
};

} //namespace t3

TEST_CASE("Call emitter as class member") {
    t3::A a;
    a.increment(5);
    CHECK_EQ(a.counter(), 5);
}

template <template <typename ...> class EmitterType>
void check_copy_move_constructors_assignments ()
{
    // Copy and move constructors
    {
        int accumulator {0};
        auto detector = [& accumulator] { accumulator++; };

        EmitterType<> em_orig;

        em_orig.connect(detector);

        em_orig(); // +1
        CHECK_EQ(accumulator, 1);

        EmitterType<> em_copy{em_orig};

        em_copy(); // +1
        CHECK_EQ(accumulator, 2);

        EmitterType<> em_moved{std::move(em_orig)};

        em_orig();  // nothing done, no detectors
        em_copy();  // +1
        em_moved(); // +1
        CHECK_FALSE(em_orig.has_detectors());
        CHECK(em_copy.has_detectors());
        CHECK_EQ(accumulator, 4);
    }

    // Copy and move assignments
    {
        int accumulator {0};
        auto detector = [& accumulator] { accumulator++; };

        EmitterType<> em_orig;
        EmitterType<> em_copy;
        EmitterType<> em_moved;

        em_orig.connect(detector);

        em_orig(); // +1
        CHECK_EQ(accumulator, 1);

        em_copy = em_orig;

        em_copy(); // +1
        CHECK_EQ(accumulator, 2);

        em_moved = std::move(em_orig);

        em_orig();  // nothing done, no detectors
        em_copy();  // +1
        em_moved(); // +1
        CHECK_FALSE(em_orig.has_detectors());
        CHECK(em_copy.has_detectors());
        CHECK_EQ(accumulator, 4);
    }
}

TEST_CASE("Emitter copy and move constructors/assignments") {
    check_copy_move_constructors_assignments<pfs::emitter>();
    check_copy_move_constructors_assignments<pfs::emitter_mt>();
    check_copy_move_constructors_assignments<pfs::emitter_mt_atomic>();
    check_copy_move_constructors_assignments<pfs::emitter_mt_spinlock>();
}

template <template <typename ...> class EmitterType>
void check_multithreading ()
{
    std::atomic<int> accumulator {0};
    std::thread th1;
    std::thread th2;

    EmitterType<> em_orig;
    EmitterType<> em_moved;// = std::move(em_orig); // Check movable constructor
    EmitterType<> em;
    em = std::move(em_moved); // Check movable assignment

    auto worker = [& em, & accumulator] {
        for (int i = 0; i < 1000; i++) {
            auto it = em.connect([& accumulator] {
                ++accumulator;
            });

            em();

            REQUIRE(em.size() > 0);

            em.disconnect(it);
        }
    };

    th1 = std::thread([worker] {
        worker();
    });

    th2 = std::thread([worker] {
        worker();
    });

    th1.join();
    th2.join();

    std::cout << "Accumulator: " << accumulator << std::endl;
}

TEST_CASE("Emitter in multithreading environment") {
    check_multithreading<pfs::emitter_mt>();
    check_multithreading<pfs::emitter_mt_atomic>();
    check_multithreading<pfs::emitter_mt_atomic_mod>();
    check_multithreading<pfs::emitter_mt_spinlock>();
    check_multithreading<pfs::emitter_mt_fast>();
}

template <template <typename ...> class EmitterType>
void benchmark_op ()
{
    t0::A a;
    EmitterType<int, std::string const &> em;
    em.connect(a, & t0::A::benchmark);

    for (int i = 0, count = std::numeric_limits<uint16_t>::max(); i < count; i++) {
        em(42, "Viva la PFS");
    }
}

// |               ns/op |                op/s |    err% |     total | benchmark
// |--------------------:|--------------------:|--------:|----------:|:----------
// |        1,517,949.00 |              658.78 |    2.2% |      0.02 | `direct`
// |        1,587,723.00 |              629.83 |    0.4% |      0.02 | `lambda`
// |        3,231,233.00 |              309.48 |    0.4% |      0.04 | `std::function`
// |        4,625,526.00 |              216.19 |    0.6% |      0.05 | `emitter ST`
// |        6,750,692.00 |              148.13 |    0.6% |      0.07 | `emitter MT (std::mutex based)`
// |        6,892,076.00 |              145.09 |    0.4% |      0.08 | `emitter MT (atomic-based)`
// |        6,501,288.00 |              153.82 |    2.1% |      0.07 | `emitter MT (atomic-based modified)`
// |        6,355,174.00 |              157.35 |    0.8% |      0.07 | `emitter MT (spinlock-based)`
// |        7,003,257.00 |              142.79 |    0.6% |      0.08 | `emitter MT (fast_mutex-based)`
//
TEST_CASE("benchmark") {
    ankerl::nanobench::Bench().run("direct", [] {
        t0::A a;

        for (int i = 0, count = std::numeric_limits<uint16_t>::max(); i < count; i++) {
            a.benchmark(42, "Viva la PFS");
        }
    });

    ankerl::nanobench::Bench().run("lambda", [] {
        t0::A a;
        auto f = [& a] (int x, std::string const & s) { a.benchmark(x, s); };

        for (int i = 0, count = std::numeric_limits<uint16_t>::max(); i < count; i++) {
            f(42, "Viva la PFS");
        }
    });

    ankerl::nanobench::Bench().run("std::function", [] {
        t0::A a;
        std::function<void (int, std::string const &)> f = [& a] (int x, std::string const & s) {
            a.benchmark(x, s);
        };

        for (int i = 0, count = std::numeric_limits<uint16_t>::max(); i < count; i++) {
            f(42, "Viva la PFS");
        }
    });

    ankerl::nanobench::Bench().run("emitter ST", benchmark_op<pfs::emitter>); //[] {
    ankerl::nanobench::Bench().run("emitter MT (std::mutex based)", benchmark_op<pfs::emitter_mt>);
    ankerl::nanobench::Bench().run("emitter MT (atomic-based)", benchmark_op<pfs::emitter_mt_atomic>);
    ankerl::nanobench::Bench().run("emitter MT (atomic-based modified)", benchmark_op<pfs::emitter_mt_atomic_mod>);
    ankerl::nanobench::Bench().run("emitter MT (spinlock-based)", benchmark_op<pfs::emitter_mt_spinlock>);
    ankerl::nanobench::Bench().run("emitter MT (fast_mutex-based)", benchmark_op<pfs::emitter_mt_fast>);
}
