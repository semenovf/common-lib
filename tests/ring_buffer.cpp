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

template <typename T>
struct BulksContainer
{
    using value_type = T;
    using bulk_type = std::vector<value_type>;
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
    int x {0};
    bool moved{false};

    X () {};
    X (int i) : x(i) {}
    X (X && other) { other.moved = true; x = other.x; other.x = -1; }
    ~X () {
//         if (!moved)
//             std::cout << "Destruct X{" << x << "}\n";
    }
};

TEST_CASE("Bulks iterator") {
    using iterator = pfs::ring_buffer_details::forward_iterator<BulksContainer<int>>;

    {
        iterator it; // default constructor
    }

    {
        std::list<std::vector<int>> bulk_list;
        static size_t const bulk_count = 5;
        static size_t const bulk_size = 1;

        for (size_t i = 0; i < bulk_count; i++) {
            bulk_list.emplace_back();
            bulk_list.back().resize(bulk_size);
        }

        BulksContainer<int> bulks{bulk_list};

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
    using ring_buffer = pfs::ring_buffer<X>;

    // Default constructor
    {
        ring_buffer rb;

        CHECK(rb.size() == 0);
        CHECK(rb.empty());
        CHECK(rb.bulk_size() == ring_buffer::default_bulk_size);
        CHECK(rb.bulk_count() == 1);
    }

    // Custom constructor
    {
        ring_buffer rb{10, 10};

        CHECK(rb.size() == 0);
        CHECK(rb.empty());
        CHECK(rb.bulk_size() == 10);
        CHECK(rb.bulk_count() == 10);
    }

    // Move constructor
    {
        ring_buffer rb_orig{10, 10};

        CHECK(rb_orig.bulk_size() == 10);
        CHECK(rb_orig.bulk_count() == 10);

        ring_buffer rb{std::move(rb_orig)};

        CHECK(rb_orig.bulk_size() == 0);
        CHECK(rb_orig.bulk_count() == 0);

        CHECK(rb.bulk_size() == 10);
        CHECK(rb.bulk_count() == 10);
    }

    // Move assignment
    {
        ring_buffer rb_orig{10, 10};
        ring_buffer rb;

        CHECK(rb_orig.bulk_size() == 10);
        CHECK(rb_orig.bulk_count() == 10);

        CHECK(rb.bulk_count() == 1);

        rb = std::move(rb_orig);

        CHECK(rb_orig.bulk_size() == 0);
        CHECK(rb_orig.bulk_count() == 0);

        CHECK(rb.bulk_size() == 10);
        CHECK(rb.bulk_count() == 10);
    }
}

TEST_CASE("Modifiers") {
    using ring_buffer = pfs::ring_buffer<X>;

    {
        ring_buffer rb{1, 3};

        REQUIRE_THROWS_AS(rb.back(), std::out_of_range);

        rb.push(X{42});

        CHECK(rb.front().x == 42);
        CHECK(rb.back().x == 42);

        rb.push(X{43});

        CHECK(rb.front().x == 42);
        CHECK(rb.back().x == 43);

        rb.push(X{44});

        CHECK(rb.front().x == 42);
        CHECK(rb.back().x == 44);

        REQUIRE_THROWS_AS(rb.push(X{45}), std::bad_alloc);

        CHECK(rb.front().x == 42);

        rb.pop();

        CHECK(rb.front().x == 43);

        rb.pop();

        CHECK(rb.front().x == 44);

        rb.pop();

        REQUIRE_THROWS_AS(rb.front(), std::out_of_range);
    }

    {
        ring_buffer rb{1, 3};

        rb.push(X{42});
        rb.push(X{43});
        rb.push(X{44});

        CHECK(rb.size() == 3);

        rb.clear();
        CHECK(rb.empty());

        rb.push(X{42});
        rb.push(X{43});
        rb.emplace(X{44});

        CHECK(rb.back().x == 44);
        CHECK(rb.size() == 3);
    }
}

TEST_CASE("Splice") {
    using ring_buffer = pfs::ring_buffer<X>;

    // Both are empty
    {
        ring_buffer rb{1, 3};
        ring_buffer rb1{1, 2};

        CHECK(rb.bulk_count() == 3);
        CHECK(rb.empty());
        CHECK(rb1.bulk_count() == 2);

        rb.splice(std::move(rb1));

        CHECK(rb.bulk_count() == 5);
        CHECK(rb.empty());
        CHECK(rb1.bulk_count() == 0);
    }
}
