#include "worthy/map.h"
#include "worthy/runtime.h"
#include "worthy/value.h"

#include <catch.hpp>


using worthy::Runtime;
using worthy::Map;
using worthy::Type;
using worthy::Value;


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
}
