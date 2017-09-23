#include "internal/eternity.h"
#include "internal/hash.h"
#include "internal/hashmap.h"
#include "internal/heap.h"

#include <catch.hpp>


using worthy::internal::HashMap;
using worthy::internal::HashMapNode;
using worthy::internal::Heap;
using worthy::internal::hash;


namespace {


constexpr std::uint32_t shift0_colliding_key_1 = 62;
constexpr std::uint32_t shift0_colliding_key_2 = 65;
constexpr std::uint32_t shift0_colliding_key_3 = 70;


struct TestContext {
    Heap heap;

    TestContext() {
        heap.lock();
    }

    ~TestContext() {
        heap.unlock();
    }

    HashMap* emptyHashMap() {
        return heap.eternity().emptyHashMap();
    }
};


} // namespace


#if 0
TEST_CASE("find colliding keys") {
    for (std::uint32_t i = 0; i < 256; i++) {
        if ((hash(i) & 0x1f) == 1) {
            CHECK(i != i);
        }
    }
}
#endif


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
    TestContext context;
    HashMap* map = context.emptyHashMap();

    REQUIRE(map->count() == 0);
}


TEST_CASE("get non-existing key", "[hashmap]") {
    TestContext context;
    HashMap* map = context.emptyHashMap();

    REQUIRE(map->get(42).isNull());
    REQUIRE(map->get(42, true) == true);
}


TEST_CASE("add values", "[hashmap]") {
    TestContext context;
    HashMap* map = context.emptyHashMap();

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
    TestContext context;
    HashMap* map = context.emptyHashMap();

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
    TestContext context;
    HashMap* map = context.emptyHashMap();

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
    TestContext context;
    HashMap* map = context.emptyHashMap();

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
    TestContext context;
    HashMap* map = context.emptyHashMap();

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

        for (int i = 0; i < 25; ++i) {
            REQUIRE(result->get(i) == i + 1);
        }
        for (int i = 26; i < 50; ++i) {
            REQUIRE(result->get(i) == i + 1);
        }
    }
}


TEST_CASE("Two HashMaps are equal", "[hashmap]") {
    TestContext context;
    HashMap* map1 = context.emptyHashMap();
    HashMap* map2 = context.emptyHashMap();

    SECTION("when they are identical") {
        REQUIRE(map1->equals(map2));
        REQUIRE(map2->equals(map1));
    }

    SECTION("unless they differ in size") {
        map1 = map1->add(nullptr, 1);

        REQUIRE_FALSE(map1->equals(map2));
        REQUIRE_FALSE(map2->equals(map1));
    }

    SECTION("when they contain same values for null key") {
        map1 = map1->add(nullptr, 1);
        map2 = map2->add(nullptr, 1);

        REQUIRE(map1->equals(map2));
        REQUIRE(map2->equals(map1));
    }

    SECTION("unless they contain different values for null key") {
        map1 = map1->add(nullptr, 1);
        map2 = map2->add(nullptr, 2);

        REQUIRE_FALSE(map1->equals(map2));
        REQUIRE_FALSE(map2->equals(map1));
    }

    SECTION("when they contain same values for same key") {
        map1 = map1->add(1, 1);
        map2 = map2->add(1, 1);

        REQUIRE(map1->equals(map2));
        REQUIRE(map2->equals(map1));
    }

    SECTION("unless they contain different values for same key") {
        map1 = map1->add(1, 1);
        map2 = map2->add(1, 2);

        REQUIRE_FALSE(map1->equals(map2));
        REQUIRE_FALSE(map2->equals(map1));
    }

    SECTION("unless they contain different keys") {
        map1 = map1->add(1, 1);
        map2 = map2->add(2, 1);

        REQUIRE_FALSE(map1->equals(map2));
        REQUIRE_FALSE(map2->equals(map1));
    }

    SECTION("when they contain the same many entries") {
        for (int i = 0; i < 50; i++) {
            map1 = map1->add(i, i + 1);
            map2 = map2->add(i, i + 1);
        }

        REQUIRE(map1->equals(map2));
        REQUIRE(map2->equals(map1));
    }

    SECTION("unless they contain different many entries") {
        for (int i = 0; i < 50; i++) {
            map1 = map1->add(i, i + 1);
            map2 = map2->add(i, i + 2);
        }

        REQUIRE_FALSE(map1->equals(map2));
        REQUIRE_FALSE(map2->equals(map1));
    }
}


TEST_CASE("The HashCodes of two HashMaps", "[hashmap]") {
    TestContext context;
    HashMap* map1 = context.emptyHashMap();
    HashMap* map2 = context.emptyHashMap();

    SECTION("are equal for identical maps") {
        REQUIRE(map1->hashCode() == map2->hashCode());
    }

    SECTION("are equal for small equal maps") {
        map1 = map1->add(nullptr, 1);
        map2 = map2->add(nullptr, 1);

        REQUIRE(map1->hashCode() == map2->hashCode());

        map1 = map1->add(1, 2);
        map2 = map2->add(1, 2);

        REQUIRE(map1->hashCode() == map2->hashCode());
    }

    SECTION("are unequal for small unequal maps") {
        map1 = map1->add(nullptr, 1);
        map2 = map2->add(nullptr, 2);

        REQUIRE(map1->hashCode() != map2->hashCode());
    }

    SECTION("are equal for large equal maps") {
        for (int i = 0; i < 50; i++) {
            map1 = map1->add(i, i + 1);
            map2 = map2->add(i, i + 1);
        }

        REQUIRE(map1->hashCode() == map2->hashCode());
    }
}
