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


TEST_CASE("associate value with null key", "[map]") {
    Runtime rt;

    Map map = rt.map();
    map = map.assoc(Value(), 42);

    REQUIRE(map.size() == 1);
    REQUIRE(map.containsKey(Value()));
    REQUIRE(map.get(Value()) == 42);
}

