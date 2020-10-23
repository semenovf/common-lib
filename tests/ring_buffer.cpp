////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of [pfs-common](https://github.com/semenovf/pfs-common) library.
//
// Changelog:
//      2020.10.21 Initial version
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/ring_buffer.hpp"
#include <list>
#include <vector>
#include <cassert>

template <typename T, size_t BulkSize>
struct BulksContainer
{
    using value_type = T;
    using bulk_type = pfs::ring_buffer_details::static_vector<value_type, BulkSize>;
    using bulk_list_type = std::list<bulk_type>;
    using size_type = std::size_t;

    using item_iterator = typename bulk_type::iterator;
    using bulk_iterator = typename bulk_list_type::iterator;

    bulk_list_type * _pbulks;

    BulksContainer (bulk_list_type & bulks)
    {
        _pbulks = & bulks;
    }

    // Iterator specific helper method
    bulk_iterator last_bulk () const
    {
        return --_pbulks->end();
    }
};

struct X {
    int * px {nullptr};

    X () {};
    X (int i) : px(new int{i})
    {}

    X (X && other)
    {
        px = other.px;
        other.px = nullptr;
    }

    int x () const
    {
        assert(px);
        return *px;
    }

    ~X ()
    {
        //std::cout << "Destruct X: ptr=" << reinterpret_cast<intptr_t>(px) << "\n";
        if (px) {
            *px = 12345;
            delete px;
        }
        px = nullptr;
    }
};

TEST_CASE("Bulks iterator") {
    static constexpr size_t const bulk_size = 1;

    using iterator = pfs::ring_buffer_details::forward_iterator<BulksContainer<int, bulk_size>>;

    {
        iterator it; // default constructor
    }

    {
        std::list<pfs::ring_buffer_details::static_vector<int, bulk_size>> bulk_list;
        static size_t const bulk_count = 5;

        for (size_t i = 0; i < bulk_count; i++) {
            bulk_list.emplace_back();
        }

        BulksContainer<int, bulk_size> bulks{bulk_list};

        {
            iterator first{bulks, bulk_list.begin(), bulk_list.front().begin()};
            iterator last{bulks, --bulk_list.end(), bulk_list.back().end()};

            while (first != last) {
                *first = 0;
                ++first;
            }

            CHECK((first == last));
        }

        int counter = 0;

        for (auto const & bulk: bulk_list) {
            if (bulk[0] != 0)
                break;
            counter++;
        }

        CHECK(counter == bulk_count);

        {
            iterator first{bulks, bulk_list.begin(), bulk_list.front().begin()};
            iterator last{bulks, --bulk_list.end(), bulk_list.back().end()};

            while (first != last) {
                *first = 42;
                ++first;
            }
        }

        counter = 0;

        for (auto const & bulk: bulk_list) {
            if (bulk[0] != 42)
                break;
            counter++;
        }

        CHECK(counter == bulk_count);
    }
}

TEST_CASE("Constructors") {
    static constexpr size_t const bulk_size = 2;
    using ring_buffer = pfs::ring_buffer<X, bulk_size>;

    // Default constructor
    {
        ring_buffer rb;

        CHECK(rb.size() == 0);
        CHECK(rb.empty());
        CHECK(rb.bulk_count() == 1);
    }

    // Custom constructor
    {
        ring_buffer rb{10};

        CHECK(rb.size() == 0);
        CHECK(rb.empty());
        CHECK(rb.capacity() == bulk_size * rb.bulk_count());
        CHECK(rb.bulk_count() == 10);
    }

    // Move constructor
    {
        ring_buffer rb_orig{10};

        CHECK(rb_orig.bulk_count() == 10);

        ring_buffer rb{std::move(rb_orig)};

        CHECK(rb_orig.capacity() == 0);
        CHECK(rb_orig.bulk_count() == 0);

        CHECK(rb.capacity() == bulk_size * 10);
        CHECK(rb.bulk_count() == 10);
    }

    // Move assignment
    {
        ring_buffer rb_orig{10};
        ring_buffer rb;

        CHECK(rb_orig.capacity() == bulk_size * 10);
        CHECK(rb_orig.bulk_count() == 10);

        CHECK(rb.bulk_count() == 1);

        rb = std::move(rb_orig);

        CHECK(rb_orig.capacity() == 0);
        CHECK(rb_orig.bulk_count() == 0);

        CHECK(rb.capacity() == bulk_size * 10);
        CHECK(rb.bulk_count() == 10);
    }
}

TEST_CASE("Modifiers") {
    {
        pfs::ring_buffer<X, 1> rb{3};

        REQUIRE_THROWS_AS(rb.back(), std::out_of_range);

        rb.push(X{42});

        CHECK(rb.front().x() == 42);
        CHECK(rb.back().x() == 42);

        rb.push(X{43});

        CHECK(rb.front().x() == 42);
        CHECK(rb.back().x() == 43);

        rb.push(X{44});

        CHECK(rb.front().x() == 42);
        CHECK(rb.back().x() == 44);

        REQUIRE_THROWS_AS(rb.push(X{45}), std::bad_alloc);

        CHECK(rb.front().x() == 42);

        rb.pop();

        CHECK(rb.front().x() == 43);

        rb.pop();

        CHECK(rb.front().x() == 44);

        rb.pop();

        REQUIRE_THROWS_AS(rb.front(), std::out_of_range);
    }

    {
        pfs::ring_buffer<X, 1> rb{3};

        rb.push(X{42});
        rb.push(X{43});
        rb.push(X{44});

        REQUIRE(rb.size() == 3);
        CHECK(rb.front().x() == 42);

        rb.clear();
        CHECK(rb.empty());

        rb.push(X{42});
        rb.push(X{43});
        rb.emplace(X{44});

        CHECK(rb.back().x() == 44);
        CHECK(rb.size() == 3);
    }
}

TEST_CASE("Splice") {
    // Both are empty
    {
        pfs::ring_buffer<X, 2> rb{3};
        decltype(rb) rb1{2};

        CHECK(rb.bulk_count() == 3);
        CHECK(rb.empty());
        CHECK(rb.capacity() == 6);

        CHECK(rb1.bulk_count() == 2);
        CHECK(rb1.capacity() == 4);

        rb.splice(std::move(rb1));

        CHECK(rb.bulk_count() == 5);
        CHECK(rb.empty());
        CHECK(rb.capacity() == 10);
        CHECK(rb1.bulk_count() == 0);
    }

    // Second buffer is empty
    {
        pfs::ring_buffer<X, 2> rb{3};
        decltype(rb) rb1{2};

        for (int i = 0; i < rb.capacity(); i++) {
            rb.push(X{42 + i});
            CHECK(rb.front().x() == 42);
        }

        CHECK(rb.front().x() == 42);

        CHECK(rb.size() == 6);
        CHECK(rb.capacity() == 6);

        rb.splice(std::move(rb1));

        CHECK(rb.size() == 6);
        CHECK(rb.capacity() == 10);

        int i = 0;
        while (rb.size() < rb.capacity())
            rb.push(X{-42 - i--});

        CHECK(rb.size() == 10);

        CHECK(rb.front().x() == 42);
    }
}
