#include "internal/handle.h"
#include "internal/handle_container.h"
#include "internal/root_block_allocator.h"

#include <catch.hpp>


using namespace worthy::internal;


struct FakeObject {
    void* payload;

    operator Object*() {
        return reinterpret_cast<Object*>(this);
    }
};


TEST_CASE("Handle container", "[handle]") {
    RootBlockAllocator allocator;
    HandleContainer container(&allocator);

    FakeObject obj;

    auto handle = container.newHandle(obj);

    REQUIRE(handle->get() == obj);

    SECTION("reuse from free list") {
        auto old_handle = handle.get();
        handle.reset();

        handle = container.newHandle(obj);

        REQUIRE(handle.get() == old_handle);
    }
}
