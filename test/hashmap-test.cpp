#include "worthy/map.h"
#include "worthy/runtime.h"

#include <catch.hpp>


using worthy::Map;
using worthy::Runtime;


TEST_CASE("hash maps can be constructed", "[hashmap]") {
    Runtime runtime;

    SECTION("empty map from runtime") {
        Map map = runtime.map();

        REQUIRE(map.size() == 0);
    }
}
