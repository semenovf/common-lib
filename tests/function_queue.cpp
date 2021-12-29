////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2019.12.19 Initial version (inhereted from https://github.com/semenovf/pfs)
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/function_queue.hpp"
#include <atomic>
#include <chrono>
#include <limits>
#include <thread>

////////////////////////////////////////////////////////////////////////////////
// Test 0: using regular function
////////////////////////////////////////////////////////////////////////////////
namespace t0 {

static int counter = 0;

void func ()
{
    ++counter;
}

} // namespace t0

TEST_CASE("Function Queue: using regular function") {
    using function_queue = pfs::function_queue<>;

    function_queue q;

    int max_count = 100;

    for (int i = 0; i < max_count; ++i)
        q.push(& t0::func);

    max_count = static_cast<int>(q.count());
    q.call_all();

    CHECK(max_count > 0);
    CHECK(t0::counter == max_count);

    max_count = 100;

    function_queue q1;

    for (int i = 0; i < max_count; ++i) {
        q1.push(& t0::func);
    }

    CHECK(q1.count() == static_cast<size_t>(max_count));
}

////////////////////////////////////////////////////////////////////////////////
// Test 1: using method
////////////////////////////////////////////////////////////////////////////////
namespace t1 {

struct A
{
    static size_t max_count;
    size_t counter;

    A () : counter(0) {}

    void method ()
    {
        ++counter;
    }
};

size_t A::max_count = 100;

} // namespace t1

TEST_CASE("Function Queue: using method") {
    using function_queue = pfs::function_queue<>;
    using t1::A;
    function_queue q;
    function_queue q1;

    A a;

    for (size_t i = 0; i < A::max_count; ++i)
        q.push(& A::method, & a);

    A::max_count = q.count();
    q.call_all();

    CHECK(A::max_count > 0);
    CHECK(a.counter == A::max_count);

    A::max_count = 100;

    for (size_t i = 0; i < A::max_count; ++i)
        q1.push(& A::method, & a);

    CHECK(q1.count() == A::max_count);
}

////////////////////////////////////////////////////////////////////////////////
// Test 2:
////////////////////////////////////////////////////////////////////////////////
namespace t2 {

static int counter1 = 0;
static int counter2 = 0;
static int counter3 = 0;

void func1 ()
{
    //std::printf("func1: %d\n", counter1);
    ++counter1;
}

void func2 (int)
{
    //std::printf("func2: %d\n", counter2);
    ++counter2;
}

void func3 (int, char)
{
    //std::printf("func3: %d\n", counter3);
    ++counter3;
}

static size_t calc_threshold (size_t max)
{
    auto r = static_cast<double>(std::rand()) / RAND_MAX;
    size_t t = static_cast<size_t>(r * (max / 2));

    if (!t)
        t = 42;

    return t;
}

} // namespace t2

TEST_CASE("Function Queue: 2") {
    using function_queue = pfs::function_queue<>;

    function_queue q;

    // TODO Uncomment when reimplement active_queue's internal function representation
//     int max = std::numeric_limits<uint16_t>::max();
    int max = 5000;

    std::srand(0xACCABEAF);

    size_t limit = t2::calc_threshold(max);

    if (!limit)
        limit = 42;

    MESSAGE("Total iterations to call functions: " << max);
    MESSAGE("Limit for queue size to start calls: " << limit);

    for (int i = 0; i < max; ++i) {
        q.push(& t2::func1);
        q.push(& t2::func2, i);
        q.push(& t2::func3, i, 'W');

        if (q.size() > limit) {
            q.call_all();
            limit = t2::calc_threshold(max);
        }
    }

    q.call_all();

    CHECK(t2::counter1 == max);
    CHECK(t2::counter2 == max);
    CHECK(t2::counter3 == max);
}

////////////////////////////////////////////////////////////////////////////////
// Test 3:
////////////////////////////////////////////////////////////////////////////////
namespace t3 {

using function_queue = pfs::function_queue<>;
function_queue q;

static std::atomic_int is_finish(0);
static std::atomic_int result(0);

static std::atomic_size_t prod1_counter(0);
static std::atomic_size_t prod2_counter(0);
static std::atomic_size_t prod3_counter(0);

static std::atomic_size_t cons1_counter(0);
static std::atomic_size_t cons2_counter(0);
static std::atomic_size_t cons3_counter(0);

void func1 ()
{
    //MESSAGE("func1()");
    ++cons1_counter;
}

void func2 (int i)
{
    //MESSAGE("func2(" << i << ")");
    ++cons2_counter;
}

void func3 (int a, char b)
{
    //MESSAGE("func3(" << a << ", " << b << ")");
    ++cons3_counter;
}

class consumer
{
public:
    consumer () {}

    void run()
    {
        while (! is_finish) {
            q.call_all();
        }

        // Wait some time to complete producers
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        q.call_all();
    }
};

class producer1
{
public:
    producer1 () {}

    void run()
    {
        while (! is_finish) {
            q.push(& func1);
            ++prod1_counter;
        }
    }
};

class producer2
{
public:
    producer2 () {}

    void run()
    {
        int i = 0;

        while (! is_finish) {
            q.push(& func2, i++);
            ++prod2_counter;
        }
    }
};

class producer3
{
public:
    producer3 () {}

    void method1 (std::shared_ptr<std::string> data)
    {
        (void)data;
        ++cons3_counter;
    }

    void run ()
    {
        int i = 0;

        while (! is_finish) {
            q.push(& func3, i++, 'W');
            ++prod3_counter;
            q.push(& producer3::method1
                    , this
                    , std::shared_ptr<std::string>(new std::string("DATA")));
            ++prod3_counter;
        }
    }
};

} // namespace t3

TEST_CASE("Function Queue: 3") {

    t3::consumer cons;
    t3::producer1 prod1;
    t3::producer2 prod2;
    t3::producer3 prod3;

    std::thread th0(& t3::consumer::run, & cons);
    std::thread th1(& t3::producer1::run, & prod1);
    std::thread th2(& t3::producer2::run, & prod2);
    std::thread th3(& t3::producer3::run, & prod3);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    t3::is_finish = 1;

    th0.join();
    th1.join();
    th2.join();
    th3.join();

    CHECK(t3::result == 0);
    CHECK(t3::q.count() == 0);
    CHECK(t3::cons1_counter == t3::prod1_counter);
    CHECK(t3::cons2_counter == t3::prod2_counter);
    CHECK(t3::cons3_counter == t3::prod3_counter);

    MESSAGE("\tresult = 0x" << std::hex << t3::result.load() << std::dec << " (must be 0x0)");
    MESSAGE("\tcons1_counter(" << t3::cons1_counter.load() << ") "
         << (t3::cons1_counter == t3::prod1_counter ? "==" : "!=")
         << " prod1_counter(" << t3::prod1_counter.load() << ")");

    MESSAGE("\tcons2_counter(" << t3::cons2_counter.load() << ") "
         << (t3::cons2_counter == t3::prod2_counter ? "==" : "!=")
         << " prod2_counter(" << t3::prod2_counter.load() << ")");

    MESSAGE("\tcons3_counter(" << t3::cons3_counter.load() << ") "
         << (t3::cons3_counter == t3::prod3_counter ? "==" : "!=")
         << " prod3_counter(" << t3::prod3_counter.load() << ")");

    MESSAGE("\tq.count() == " << t3::q.count() << " (must be 0)");
}

////////////////////////////////////////////////////////////////////////////////
// Test 4:
////////////////////////////////////////////////////////////////////////////////
namespace t4 {

using function_queue = pfs::function_queue<>;
static function_queue q;

static std::atomic_int is_finish(0);
static std::atomic_int result(0);

void func1 ()
{}

void func2 ()
{
    q.push(& func1);
}

class consumer
{
public:
    consumer () {}

    void run()
    {
        while (! is_finish) {
            q.call_all();
        }
    }
};

class producer
{
    public:
    producer () {}

    void run()
    {
        while (! is_finish) {
            q.push(& func2);
        }
    }
};

} // namespace t4

TEST_CASE("Function Queue: 4") {

    t4::consumer cons;
    t4::producer prod;

    std::thread th0(& t4::consumer::run, & cons);
    std::thread th1(& t4::producer::run, & prod);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    t4::is_finish = 1;

    th1.join();
    th0.join();

    if (t4::result != 0)
        MESSAGE("result = 0x" << std::hex << t4::result.load());

    CHECK(t4::result == 0);
}

////////////////////////////////////////////////////////////////////////////////
// Test 5:
////////////////////////////////////////////////////////////////////////////////

namespace t5 {

using function_queue = pfs::function_queue<>;

static int const COUNT          = 10000;
static int const PRODUCER_COUNT = 10;
static int const CONSUMER_COUNT = 15;

static function_queue callbackq;
static std::atomic_int quit_flag(PRODUCER_COUNT);
static std::atomic_int counter(0);

void func1 ()
{
    ++counter;
}

class producer
{
public:
    producer () {}

    void run ()
    {
        for (int i = 0; i < COUNT; ++i) {
            callbackq.push(& func1);
        }

        --quit_flag;
    }
};

class consumer
{
public:
    consumer () {}

    void run ()
    {
        while (! (quit_flag.load() == 0 /*&& callbackq.empty()*/)) {
            callbackq.call_all();
        }
    }
};

} // namespace t5

TEST_CASE("Function Queue: 5")
{
    using std::swap;
    t5::producer producers[t5::PRODUCER_COUNT];
    t5::consumer consumers[t5::CONSUMER_COUNT];

    std::unique_ptr<std::thread> consumer_threads[t5::CONSUMER_COUNT];
    std::unique_ptr<std::thread> producer_threads[t5::PRODUCER_COUNT];

    for (int i = 0; i < t5::CONSUMER_COUNT; ++i) {
        std::unique_ptr<std::thread> ptr(new std::thread(& t5::consumer::run, & consumers[i]));
        swap(consumer_threads[i], ptr);
    }

    for (int i = 0; i < t5::PRODUCER_COUNT; ++i) {
        std::unique_ptr<std::thread> ptr(new std::thread(& t5::producer::run, & producers[i]));
        swap(producer_threads[i], ptr);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    for (int i = 0; i < t5::CONSUMER_COUNT; ++i)
        consumer_threads[i]->join();

    for (int i = 0; i < t5::PRODUCER_COUNT; ++i)
        producer_threads[i]->join();

    CHECK(t5::counter == t5::COUNT * t5::PRODUCER_COUNT);
}
