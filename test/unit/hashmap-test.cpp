#include "internal/hash.h"
#include "internal/hashmap.h"
#include "internal/heap.h"

#include <catch.hpp>


using worthy::internal::HashMap;
using worthy::internal::HashMapNode;
using worthy::internal::Heap;
using worthy::internal::hash;


TEST_CASE("construct empty hashmap", "[hashmap]") {
    Heap heap;
    HashMap* map = heap.emptyHashMap();

    REQUIRE(map->count() == 0);
}


TEST_CASE("get non-existing key", "[hashmap]") {
    Heap heap;
    HashMap* map = heap.emptyHashMap();

    REQUIRE(map->get(42).isNull());
    REQUIRE(map->get(42, true) == true);
}


TEST_CASE("add values", "[hashmap]") {
    Heap heap;
    HashMap* map = heap.emptyHashMap();

    HashMap* map2 = map->add(42, 1337);

    SECTION("value is added") {
        REQUIRE(map2->count() == 1);
        REQUIRE(map2->containsKey(42));
        REQUIRE(map2->get(42) == 1337);
    }

    SECTION("original map is not modified") {
        REQUIRE(map->count() == 0);
        REQUIRE(!map->containsKey(42));
    }

    SECTION("other key does not exist") {
        REQUIRE(!map2->containsKey(43));
        REQUIRE(map2->get(43).isNull());
    }

    SECTION("add same entry again") {
        HashMap* map3 = map2->add(42, 1337);

        REQUIRE(map3->count() == 1);
        REQUIRE(map3->get(42) == 1337);
    }

    SECTION("overwrite key") {
        HashMap* map3 = map2->add(42, false);

        REQUIRE(map3->count() == 1);
        REQUIRE(map3->get(42) == false);
    }
}


TEST_CASE("value with null key", "[hashmap]") {
    Heap heap;
    HashMap* map = heap.emptyHashMap();

    SECTION("add value") {
        map = map->add(nullptr, 42);

        REQUIRE(map->count() == 1);
        REQUIRE(map->containsKey(nullptr));
    }

    SECTION("get value") {
        map = map->add(nullptr, 42);

        REQUIRE(map->get(nullptr) == 42);
    }

    SECTION("overwrite value") {
        map = map->add(nullptr, 1);
        map = map->add(nullptr, 2);

        REQUIRE(map->get(nullptr) == 2);
    }

    SECTION("add value multiple times") {
        for (int i = 0; i < 10; ++i) {
            map = map->add(nullptr, 42);
        }

        REQUIRE(map->count() == 1);
    }

    SECTION("add null and non-null") {
        map = map->add(nullptr, 1);
        map = map->add(0, 2);

        REQUIRE(map->count() == 2);
        REQUIRE(map->get(nullptr) == 1);
        REQUIRE(map->get(0) == 2);
    }
}


TEST_CASE("hash collision", "[hashmap]") {
    Heap heap;
    HashMap* map = heap.emptyHashMap();

    SECTION("first shift") {
        const std::uint32_t key1 = 0;
        const std::uint32_t key2 = 5;

        REQUIRE((hash(key1) & 0x1f) == (hash(key2) & 0x1f));

        map = map->add(key1, key1)->add(key2, key2);

        REQUIRE(map->count() == 2);
        REQUIRE(map->get(key1) == key1);
        REQUIRE(map->get(key2) == key2);
    }
}


TEST_CASE("add many values", "[hashmap]") {
    Heap heap;
    HashMap* map = heap.emptyHashMap();

    SECTION("10 values") {
        const int n = 10;

        for (int i = 0; i < n; ++i) {
            map = map->add(i, i + 1);
        }

        REQUIRE(map->count() == n);

        for (int i = 0; i < n; ++i) {
            REQUIRE(map->get(i, i) == i + 1);
        }
    }

    SECTION("50 values") {
        const int n = 50;

        for (int i = 0; i < n; ++i) {
            map = map->add(i, i + 1);
        }

        REQUIRE(map->count() == n);

        for (int i = 0; i < n; ++i) {
            REQUIRE(map->get(i, i) == i + 1);
        }
    }
}
