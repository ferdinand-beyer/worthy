#include "internal/hashmap.h"
#include "internal/heap.h"

#include <catch.hpp>


using namespace worthy::internal;


TEST_CASE("Creating objects increases heap object count", "[gc]") {
    Heap heap;

    const auto initial_count = heap.objectCount();

    heap.lock();

    HashMap* hashMap = construct<HashMap>();

    REQUIRE(heap.objectCount() == (initial_count + 1));
}
