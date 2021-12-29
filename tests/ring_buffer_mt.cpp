////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2020.10.24 Initial version
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/ring_buffer.hpp"
#include <atomic>
#include <list>
#include <thread>
#include <vector>
#include <cassert>

struct X {
    int * px {nullptr};

    X () {};
    X (int i) : px(new int{i})
    {
//         std::cout << "Allocated: 0x" << std::hex << reinterpret_cast<intptr_t>(px)
//             << "; x = " << *px << "\n";
    }

    X (X && other)
    {
        px = other.px;
        other.px = nullptr;
//         std::cout << "Move constructor: 0x" << std::hex << reinterpret_cast<intptr_t>(px)
//             << "; x = " << *px << "\n";
    }

    X & operator = (X && other)
    {
        px = other.px;
        other.px = nullptr;

//         if (!px) {
//             std::cout << "Move assignment: Oops\n";
//         } else {
//             std::cout << "Move assignment: 0x" << std::hex << reinterpret_cast<intptr_t>(px)
//                 << "; x = " << *px << "\n";
//         }

        return *this;
    }

    int x () const
    {
//         std::cout << "Getter: 0x" << std::hex << reinterpret_cast<intptr_t>(px) << "\n";
//
//         if (!px) {
//             std::cout << "Getter: Oops\n";
//         }

        assert(px);
        return *px;
    }

    ~X ()
    {
//         std::cout << "Destructor: 0x" << std::hex << reinterpret_cast<intptr_t>(px) << "\n";

        if (px) {
//             std::cout << "Delete: 0x" << std::hex << reinterpret_cast<intptr_t>(px)
//                 << "; x = " << *px << "\n";
//
            *px = 12345;
            delete px;
        }
        px = nullptr;
    }
};

template <size_t BulkSize, size_t N, int Count>
bool test_producer_consumer (int nproducers, int nconsumers)
{
    pfs::ring_buffer_mt<X, BulkSize> rb{N};
    std::atomic_int counter {Count};
    std::atomic_bool quit {false};

    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    for (int i = 0; i < nproducers; i++) {
        producers.emplace_back([& rb, & counter, & quit] () {
            while (counter > 0) {
                if (rb.try_emplace(42)) {
                    --counter;
                }
            }

            quit = true;
        });
    }

    for (int i = 0; i < nconsumers; i++) {
        consumers.emplace_back([& rb, & quit] () {
            while (!quit) {
                X x;
                if (rb.try_pop(x)) {
                    CHECK(x.x() == 42);
                }
            }
        });
    }

    for (auto & p: producers)
        p.join();

    for (auto & c: consumers)
        c.join();

    return counter <= 0;
}

TEST_CASE("Multithreading") {
    static constexpr int counter = 100000;

    CHECK_MESSAGE((test_producer_consumer<32, 5, counter>(1, 1))
        , "One producer / One consumer");

    CHECK_MESSAGE((test_producer_consumer<32, 5, counter>(1, 5))
        , "One producer / N consumers");

    CHECK_MESSAGE((test_producer_consumer<32, 5, counter>(5, 1))
        , "N producers / One consumers");

    CHECK_MESSAGE((test_producer_consumer<32, 5, counter>(4, 6))
        , "N producers / N consumers");

    CHECK_MESSAGE((test_producer_consumer<32, 5, counter>(6, 4))
        , "N producers / N consumers");
}


template <size_t BulkSize, size_t N, int Count>
bool test_producer_consumer_expanded (int nproducers
    , int nconsumers
    , int capacity_inc)
{
    pfs::ring_buffer_mt<X, BulkSize> rb{N};
    std::atomic_int counter {Count};
    std::atomic_bool quit {false};

    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    MESSAGE("Initial capacity: " << rb.capacity() << "\n");

    for (int i = 0; i < nproducers; i++) {
        producers.emplace_back([& rb, & counter, & quit, capacity_inc] () {
            while (counter > 0) {
                if (rb.try_emplace(42)) {
                    rb.reserve(rb.capacity() + static_cast<size_t>(capacity_inc));
                    REQUIRE(rb.try_emplace(42));
                    --counter;
                }
            }

            quit = true;
        });
    }

    for (int i = 0; i < nconsumers; i++) {
        consumers.emplace_back([& rb, & quit] () {
            while (!quit) {
                X x;
                if (rb.try_pop(x)) {
                    CHECK(x.x() == 42);
                }
            }
        });
    }

    for (auto & p: producers)
        p.join();

    for (auto & c: consumers)
        c.join();

    MESSAGE("Result capacity: " << rb.capacity() << "\n");
    return counter <= 0;
}

TEST_CASE("Multithreading with expandable buffer") {
        static constexpr int counter = 100000;

    CHECK_MESSAGE((test_producer_consumer_expanded<32, 5, counter>(1, 1, 10))
        , "One producer / One consumer");

    CHECK_MESSAGE((test_producer_consumer_expanded<32, 5, counter>(1, 5, 10))
        , "One producer / N consumers");

    CHECK_MESSAGE((test_producer_consumer_expanded<32, 5, counter>(5, 1, 10))
        , "N producers / One consumers");

    CHECK_MESSAGE((test_producer_consumer_expanded<32, 5, counter>(4, 6, 10))
        , "N producers / N consumers");

    CHECK_MESSAGE((test_producer_consumer_expanded<32, 5, counter>(6, 4, 10))
        , "N producers / N consumers");
}

template <size_t BulkSize, size_t N, int Count>
bool test_producer_consumer_wait (int nproducers
    , int nconsumers
    , int capacity_inc)
{
    pfs::ring_buffer_mt<X, BulkSize> rb{N};
    std::atomic_int counter {Count};

    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    int quit_flag = 1042;

    MESSAGE("Initial capacity: " << rb.capacity() << "\n");

    for (int i = 0; i < nproducers; i++) {
        producers.emplace_back([& rb, & counter, quit_flag, capacity_inc, nconsumers] () {
            while (counter > 0) {
                if (rb.try_emplace(42)) {
                    rb.reserve(rb.capacity() + static_cast<size_t>(capacity_inc));
                    REQUIRE(rb.try_emplace(42));
                    --counter;
                }
            }

            for (int i = 0; i < nconsumers; i++)
                rb.try_emplace(quit_flag);
        });


    }

    for (int i = 0; i < nconsumers; i++) {
        consumers.emplace_back([& rb, quit_flag] () {
            while (true) {
                X x;
                rb.wait();
                if (rb.try_pop(x)) {
                    if (x.x() == quit_flag)
                        break;
                    CHECK(x.x() == 42);
                }
            }
        });
    }

    for (auto & p: producers)
        p.join();

    for (auto & c: consumers)
        c.join();

    MESSAGE("Result capacity: " << rb.capacity() << "\n");
    return counter <= 0;
}

TEST_CASE("Multithreading with wait()") {
        static constexpr int counter = 100000;

    CHECK_MESSAGE((test_producer_consumer_wait<32, 5, counter>(1, 1, 10))
        , "One producer / One consumer");

    CHECK_MESSAGE((test_producer_consumer_wait<32, 5, counter>(1, 5, 10))
        , "One producer / N consumers");

    CHECK_MESSAGE((test_producer_consumer_wait<32, 5, counter>(5, 1, 10))
        , "N producers / One consumers");

    CHECK_MESSAGE((test_producer_consumer_wait<32, 5, counter>(4, 6, 10))
        , "N producers / N consumers");

    CHECK_MESSAGE((test_producer_consumer_wait<32, 5, counter>(6, 4, 10))
        , "N producers / N consumers");
}
