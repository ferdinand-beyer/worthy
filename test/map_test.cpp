#include "worthy/map.h"
#include "worthy/runtime.h"
#include "worthy/value.h"

#include <catch.hpp>


using namespace worthy;


TEST_CASE("An empty Map is empty", "[api][map]") {
    Runtime rt;

    Map map = rt.map();

    REQUIRE(map.isEmpty());
    REQUIRE(map.size() == 0);
}


TEST_CASE("A Map returns a null or default value for non-existing key", "[api][map]") {
    Runtime rt;

    Map map = rt.map();

    REQUIRE(map.get(42).isNull());
    REQUIRE(map.get(42, true) == true);
}


TEST_CASE("When adding a value to a Map", "[api][map]") {
    Runtime rt;

    Map map = rt.map();
    Map map2 = map.add(42, 1337);

    SECTION("the size is changed") {
        REQUIRE(map2.size() == 1);
    }

    SECTION("the value can be retrieved") {
        REQUIRE(map2.containsKey(42));
        REQUIRE(map2.get(42) == 1337);
    }

    SECTION("the original map is not modified") {
        REQUIRE(map.size() == 0);
        REQUIRE(!map.containsKey(42));
    }

    SECTION("another key does not exist") {
        REQUIRE(!map2.containsKey(43));
        REQUIRE(map2.get(43).isNull());
    }

    SECTION("twice does has no effect") {
        Map map3 = map2.add(42, 1337);

        REQUIRE(map3.size() == 1);
        REQUIRE(map3.get(42) == 1337);
    }

    SECTION("with the same key, the old value is overwritten") {
        Map map3 = map2.add(42, false);

        REQUIRE(map3.size() == 1);
        REQUIRE(map3.get(42) == false);
    }
}


TEST_CASE("When adding a null key to a Map", "[api][map]") {
    Runtime rt;

    Map map = rt.map();

    SECTION("the value is associated") {
        map = map.add(nullptr, 42);

        REQUIRE(map.size() == 1);
        REQUIRE(map.containsKey(nullptr));
        REQUIRE(map.get(nullptr) == 42);
    }

    SECTION("an old value is overwritten") {
        map = map.add(nullptr, 1);
        map = map.add(nullptr, 2);

        REQUIRE(map.get(nullptr) == 2);
    }

    SECTION("multiple times has no effect") {
        for (int i = 0; i < 10; ++i) {
            map = map.add(nullptr, 42);
        }

        REQUIRE(map.size() == 1);
    }

    SECTION("it does not effect non-null keys") {
        map = map.add(nullptr, 1);
        map = map.add(0, 2);

        REQUIRE(map.size() == 2);
        REQUIRE(map.get(nullptr) == 1);
        REQUIRE(map.get(0) == 2);
    }
}


TEST_CASE("Map supports adding multiple values", "[api][map]") {
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
            map = map.add(i, i + 1);
        }

        REQUIRE(map.size() == n);

        for (int i = 0; i < n; ++i) {
            REQUIRE(map.get(i, i) == i + 1);
        }
    }
}
