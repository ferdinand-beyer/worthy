#include "internal/block-allocator.h"
#include "internal/block-constants.h"

#include <catch.hpp>

#include <set>


using namespace worthy::internal;


TEST_CASE("Allocate single blocks", "[block]") {
    BlockAllocator allocator;

    SECTION("just one") {
        Block* block = allocator.allocateBlock();

        REQUIRE(block);
    }

    SECTION("from the same chunk") {
        std::set<Block*> blocks;

        for (int i = 0; i < BlocksPerChunk; i++) {
            Block* b = allocator.allocateBlock();

            auto inserted = blocks.insert(b);
            REQUIRE(inserted.second);

            REQUIRE(BlockSize == b->bytesAvailable());
        }

        REQUIRE(1 == allocator.chunksAllocated());
    }

    SECTION("more than fit in one chunk") {
        for (int i = 0; i < (BlocksPerChunk + 1); i++) {
            allocator.allocateBlock();
        }

        REQUIRE(2 == allocator.chunksAllocated());
    }
}
