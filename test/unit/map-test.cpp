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

    REQUIRE(map.size() == 0);
}


TEST_CASE("add values", "[map]") {
    Runtime rt;

    Map map1 = rt.map();
    Map map2 = map1.add(42, 1337);

    REQUIRE(map2.size() == 1);
    REQUIRE(map2.containsKey(42));
    REQUIRE(map2.get(42) == 1337);

    REQUIRE(map1.size() == 0);
    REQUIRE(!map1.containsKey(42));
}


TEST_CASE("add value with null key", "[map]") {
    Runtime rt;

    Map map = rt.map();
    map = map.add(nullptr, 42);

    REQUIRE(map.size() == 1);
    REQUIRE(map.containsKey(Value()));
    REQUIRE(map.get(nullptr) == 42);
}

