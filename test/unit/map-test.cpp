#include "worthy/map.h"
#include "worthy/runtime.h"
#include "worthy/value.h"
#include "internal/hash.h"

#include <catch.hpp>


using worthy::Map;
using worthy::Runtime;
using worthy::Type;
using worthy::Value;
using worthy::internal::hash;


TEST_CASE("construct empty map", "[map]") {
    Runtime rt;

    Map map = rt.map();

    REQUIRE(map.isEmpty());
    REQUIRE(map.size() == 0);
}


TEST_CASE("get non-existing key", "[map]") {
    Runtime rt;

    Map map = rt.map();

    REQUIRE(map.get(42).isNull());
    REQUIRE(map.get(42, true) == true);
}


TEST_CASE("add values", "[map]") {
    Runtime rt;

    Map map = rt.map();
    Map map2 = map.add(42, 1337);

    SECTION("value is added") {
        REQUIRE(map2.size() == 1);
        REQUIRE(map2.containsKey(42));
        REQUIRE(map2.get(42) == 1337);
    }

    SECTION("original map is not modified") {
        REQUIRE(map.size() == 0);
        REQUIRE(!map.containsKey(42));
    }

    SECTION("other key does not exist") {
        REQUIRE(!map2.containsKey(43));
        REQUIRE(map2.get(43).isNull());
    }

    SECTION("add same entry again") {
        Map map3 = map2.add(42, 1337);

        REQUIRE(map3.size() == 1);
        REQUIRE(map3.get(42) == 1337);
    }

    SECTION("overwrite key") {
        Map map3 = map2.add(42, false);

        REQUIRE(map3.size() == 1);
        REQUIRE(map3.get(42) == false);
    }
}


TEST_CASE("value with null key", "[map]") {
    Runtime rt;

    Map map = rt.map();

    SECTION("add value") {
        map = map.add(nullptr, 42);

        REQUIRE(map.size() == 1);
        REQUIRE(map.containsKey(nullptr));
    }

    SECTION("get value") {
        map = map.add(nullptr, 42);

        REQUIRE(map.get(nullptr) == 42);
    }

    SECTION("overwrite value") {
        map = map.add(nullptr, 1);
        map = map.add(nullptr, 2);

        REQUIRE(map.get(nullptr) == 2);
    }

    SECTION("add value multiple times") {
        for (int i = 0; i < 10; ++i) {
            map = map.add(nullptr, 42);
        }

        REQUIRE(map.size() == 1);
    }

    SECTION("add null and non-null") {
        map = map.add(nullptr, 1);
        map = map.add(0, 2);

        REQUIRE(map.size() == 2);
        REQUIRE(map.get(nullptr) == 1);
        REQUIRE(map.get(0) == 2);
    }
}


TEST_CASE("hash collision", "[map]") {
    Runtime rt;

    Map map = rt.map();

    SECTION("first shift") {
        const std::uint32_t key1 = 0;
        const std::uint32_t key2 = 5;

        REQUIRE((hash(key1) & 0x1f) == (hash(key2) & 0x1f));

        map = map.add(key1, key1).add(key2, key2);

        REQUIRE(map.size() == 2);
        REQUIRE(map.get(key1) == key1);
        REQUIRE(map.get(key2) == key2);
    }
}


TEST_CASE("add many values", "[map]") {
    Runtime rt;

    Map map = rt.map();

    SECTION("10 values") {
        const int n = 10;

        for (int i = 0; i < n; ++i) {
            map = map.add(i, i);
        }

        REQUIRE(map.size() == n);

        for (int i = 0; i < n; ++i) {
            REQUIRE(map.get(i) == i);
        }
    }

    SECTION("50 values") {
        const int n = 50;

        for (int i = 0; i < n; ++i) {
            map = map.add(i, i);
        }

        REQUIRE(map.size() == n);

        for (int i = 0; i < n; ++i) {
            REQUIRE(map.get(i) == i);
        }
    }

    SECTION("10,000 values") {
        const int n = 10000;

        for (int i = 0; i < n; ++i) {
            map = map.add(i, i);
        }

        REQUIRE(map.size() == n);

        for (int i = 0; i < n; ++i) {
            REQUIRE(map.get(i) == i);
        }
    }
}
