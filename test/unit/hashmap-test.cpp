#include "internal/hash.h"
#include "internal/hashmap.h"
#include "internal/heap.h"

#include <catch.hpp>


using worthy::internal::HashMap;
using worthy::internal::HashMapNode;
using worthy::internal::Heap;
using worthy::internal::hash;


constexpr std::uint32_t shift0_colliding_key_1 = 0;
constexpr std::uint32_t shift0_colliding_key_2 = 5;
constexpr std::uint32_t shift0_colliding_key_3 = 24;


TEST_CASE("peconditions") {
    SECTION("shift0 colliding keys") {
        const auto hash1 = hash(shift0_colliding_key_1);
        const auto hash2 = hash(shift0_colliding_key_2);
        const auto hash3 = hash(shift0_colliding_key_3);

        REQUIRE((hash1 & 0x1f) == (hash2 & 0x1f));
        REQUIRE((hash1 & 0x1f) == (hash3 & 0x1f));
    }
}


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

    SECTION("first level") {
        constexpr auto key1 = shift0_colliding_key_1;
        constexpr auto key2 = shift0_colliding_key_2;

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


TEST_CASE("remove values", "[hashmap]") {
    Heap heap;
    HashMap* map = heap.emptyHashMap();

    SECTION("remove non-existing null key") {
        HashMap* result = map->remove(nullptr);

        REQUIRE(result == map);
    }

    SECTION("remove existing null key") {
        map = map->add(nullptr, 42);

        HashMap* result = map->remove(nullptr);

        REQUIRE(result->count() == 0);
    }

    SECTION("remove non-null key from empty map") {
        HashMap* result = map->remove(17);

        REQUIRE(result == map);
    }

    SECTION("remove non-existing non-null key") {
        map = map->add(8, 15);

        HashMap* result = map->remove(17);

        REQUIRE(result == map);
    }

    SECTION("remove only key") {
        map = map->add(8, 15);

        HashMap* result = map->remove(8);

        REQUIRE(result->count() == 0);
    }

    SECTION("remove existing non-null key") {
        map = map->add(17, 25)->add(23, 32);

        HashMap* result = map->remove(17);

        REQUIRE(result->count() == 1);
        REQUIRE(result->get(23) == 32);
        REQUIRE(!result->containsKey(17));
    }

    SECTION("remove non-existing key with colliding first-level hash") {
        constexpr auto key1 = shift0_colliding_key_1;
        constexpr auto key2 = shift0_colliding_key_2;

        map = map->add(key1, 1);

        HashMap* result = map->remove(key2);

        REQUIRE(result == map);

        SECTION("multiple keys") {
            constexpr auto key3 = shift0_colliding_key_3;

            map = map->add(key2, 2);

            result = map->remove(key3);

            REQUIRE(result == map);
        }
    }

    SECTION("remove key with colliding first-level hashes") {
        constexpr auto key1 = shift0_colliding_key_1;
        constexpr auto key2 = shift0_colliding_key_2;

        map = map->add(key1, 1)->add(key2, 2);

        HashMap* result = map->remove(key1);

        REQUIRE(result->count() == 1);
        REQUIRE(result->get(key2) == 2);
        REQUIRE(!result->containsKey(key1));

        SECTION("remove all keys") {
            result = result->remove(key2);

            REQUIRE(result->count() == 0);
        }

        SECTION("remove all colliding keys") {
            result = result->add(1, 17)->remove(key2);

            REQUIRE(result->count() == 1);
        }
    }

    SECTION("remove from larger map") {
        for (int i = 0; i < 50; ++i) {
            map = map->add(i, i + 1);
        }

        HashMap* result = map->remove(25);

        REQUIRE(result->count() == 49);
        REQUIRE(!result->containsKey(25));
    }
}
