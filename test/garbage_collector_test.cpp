#include "internal/byte_array.h"
#include "internal/eternity.h"
#include "internal/frame.h"
#include "internal/garbage_collector.h"
#include "internal/hashmap.h"
#include "internal/heap.h"

#include <catch.hpp>

#include <array>
#include <vector>


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


TEST_CASE("Root objects survive one-block garbage collection", "[gc]") {
    Heap heap;

    heap.lock();
    HashMap* old_address = construct<HashMap>();
    HandlePtr handle = heap.makeHandle(old_address);
    heap.unlock();

    heap.gc();

    // Our object should have been evacuated to a new block, and the Handle
    // updated to the new address.
    Object* new_address = handle->get();
    REQUIRE(new_address);
    REQUIRE(new_address != old_address);
    REQUIRE(new_address->isHashMap());
}


TEST_CASE("Reachable objects survive one-block garbage collection", "[gc]") {
    Heap heap;
    const auto initial_count = heap.objectCount();

    heap.lock();
    auto map0 = construct<HashMap>();
    const auto count0 = heap.objectCount();
    auto map1 = map0->add(13, 37);
    const auto count1 = heap.objectCount();
    auto handle = heap.makeHandle(map1);
    heap.unlock();

    REQUIRE(heap.objectCount() == count1);

    heap.gc();

    REQUIRE(heap.objectCount() == (initial_count + count1 - count0));
}


TEST_CASE("Root objects survive multi-block garbage collection", "[gc]") {
    std::array<byte, 300> buffer;
    buffer.fill('\0');

    Heap heap;

    std::vector<Object*> pointers;
    std::vector<HandlePtr> handles;

    const int num_objects = 100;

    heap.lock();

    for (int i = 0; i < num_objects; i++) {
        buffer[0] = i;
        auto ba = ByteArray::valueOf(buffer.data(), buffer.size());
        pointers.push_back(ba);
        handles.push_back(heap.makeHandle(ba));
    }

    heap.unlock();
    heap.gc();

    for (int i = 0; i < num_objects; i++) {
        auto obj = handles[i]->get();

        // Object has been moved.
        REQUIRE(obj);
        REQUIRE(obj != pointers[i]);

        REQUIRE(obj->isByteArray());

        auto ba = ByteArray::cast(obj);

        // Object is still valid.
        REQUIRE(ba->size() == buffer.size());
        REQUIRE(ba->data()[0] == i);
    }
}


TEST_CASE("Reachable objects survive multi-block garbage collection", "[gc]") {
    const int num_byte_arrays = 100;

    std::array<byte, 300> buffer;
    buffer.fill('\0');

    std::vector<Object*> pointers;

    Heap heap;
    heap.lock();

    HashMap* map = heap.eternity().emptyHashMap();

    for (int i = 0; i < num_byte_arrays; i++) {
        buffer[0] = i;
        auto ba = ByteArray::valueOf(buffer.data(), buffer.size());
        pointers.push_back(ba);
        map = map->add(i, ba);
    }

    HandlePtr handle = heap.makeHandle(map);

    heap.unlock();
    heap.gc();
    heap.lock();

    map = HashMap::cast(handle->get());

    for (int i = 0; i < num_byte_arrays; i++) {
        auto val = map->get(i);

        REQUIRE(val.isObject());

        auto obj = val.toObject();

        REQUIRE(obj);
        REQUIRE(obj != pointers[i]);

        REQUIRE(obj->isByteArray());

        auto ba = ByteArray::cast(obj);

        REQUIRE(ba->size() == buffer.size());
        REQUIRE(ba->data()[0] == i);
    }

    heap.unlock();
}


TEST_CASE("Repeated garbage collection retains memory usage", "[gc]") {
    constexpr int num_collections = 10;

    std::array<byte, 1000> buffer;
    buffer.fill('\0');

    Heap heap;

    heap.lock();
    auto ba = ByteArray::valueOf(buffer.data(), buffer.size());
    auto handle = heap.makeHandle(ba);
    heap.unlock();

    const auto memory_threshold = heap.bytesAllocated();

    for (int i = 0; i < num_collections; i++) {
        heap.lock();
        ba = ByteArray::valueOf(buffer.data(), buffer.size());
        handle = heap.makeHandle(ba);
        heap.unlock();

        heap.gc();
    }

    REQUIRE(heap.bytesAllocated() <= memory_threshold);
}

