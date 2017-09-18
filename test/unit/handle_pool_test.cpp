#include "internal/handle.h"
#include "internal/handle_pool.h"
#include "internal/root_block_allocator.h"

#include <catch.hpp>


using namespace worthy::internal;


struct FakeObject {
    void* payload;

    operator Object*() {
        return reinterpret_cast<Object*>(this);
    }
};


TEST_CASE("Handle pool", "[handle]") {
    RootBlockAllocator allocator;
    HandlePool pool(&allocator);

    FakeObject obj;

    auto handle = pool.makeHandle(obj);

    REQUIRE(handle->get() == obj);

    SECTION("reuse from free list") {
        auto old_handle = handle.get();
        handle.reset();

        handle = pool.makeHandle(obj);

        REQUIRE(handle.get() == old_handle);
    }
}
