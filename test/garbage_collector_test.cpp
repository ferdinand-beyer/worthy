#include "internal/frame.h"
#include "internal/garbage_collector.h"
#include "internal/hashmap.h"
#include "internal/heap.h"

#include <catch.hpp>


using namespace worthy::internal;


TEST_CASE("Heap object count", "[gc]") {
    Heap heap;
    const auto initial_count = heap.objectCount();

    heap.lock();
    auto object = construct<HashMap>();
    heap.unlock();

    SECTION("is increased when creating objects") {
        REQUIRE(heap.objectCount() == (initial_count + 1));
    }

    SECTION("is decreased when collecting objects") {
        // We don't own a handle, so object will be collected.
        heap.gc();
        REQUIRE(heap.objectCount() == initial_count);

        SECTION("not effected by multiple collections") {
            heap.gc();
            REQUIRE(heap.objectCount() == initial_count);
        }
    }

    SECTION("is not changed when keeping a handle") {
        auto handle = heap.makeHandle(object);
        heap.gc();
        REQUIRE(heap.objectCount() == (initial_count + 1));
    }
}


TEST_CASE("Root objects survive garbage collection", "[gc]") {
    Heap heap;

    heap.lock();
    HashMap* old_address = construct<HashMap>();
    HandlePtr handle = heap.makeHandle(old_address);
    heap.unlock();

    heap.gc();

    // Our object should have been evacuated to a new block, and the Handle
    // updated to the new address.
    REQUIRE(handle->get() != old_address);
}
