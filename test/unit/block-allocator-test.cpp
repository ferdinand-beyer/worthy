#include "internal/block-allocator.h"

#include <catch.hpp>


using worthy::internal::Block;
using worthy::internal::BlockAllocator;


TEST_CASE("Allocate a single block", "[block]") {
    BlockAllocator allocator;

    Block* block = allocator.allocateBlock();

    REQUIRE(block);
}
