#include "worthy/map.h"
#include "worthy/runtime.h"

#include <catch.hpp>


using worthy::Runtime;
using worthy::Map;
using worthy::Type;


TEST_CASE("construct empty map", "[map]") {
    Runtime rt;

    Map map = rt.map();

    REQUIRE(map.size() == 0);
}

