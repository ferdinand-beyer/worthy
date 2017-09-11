#include "internal/block-allocator.h"
#include "internal/block-layout.h"

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


TEST_CASE("Allocate block groups", "[block]") {
    BlockAllocator allocator;

    SECTION("smaller than one chunk") {
        Block* block = allocator.allocateBlockGroup(10);

        REQUIRE((10 * BlockSize) == block->bytesAvailable());
    }
}


TEST_CASE("Deallocate blocks", "[block]") {
    BlockAllocator allocator;

    Block* first = allocator.allocateBlock();

    // Exhaust the first chunk.
    for (int i = 1; i < BlocksPerChunk; i++) {
        allocator.allocateBlock();
    }

    REQUIRE(1 == allocator.chunksAllocated());

    allocator.deallocate(first);

    Block* reused = allocator.allocateBlock();

    REQUIRE(1 == allocator.chunksAllocated());
    REQUIRE(first == reused);
}


TEST_CASE("Merge blocks", "[block]") {
    BlockAllocator allocator;

    Block* blocks[BlocksPerChunk];

    for (int i = 0; i < BlocksPerChunk; i++) {
        blocks[i] = allocator.allocateBlock();
    }

    // Deallocate in zig-zag to trigger forward and backward merging.
    for (int i = 0; i < 8; i++) {
        allocator.deallocate(blocks[20 + i]);
        allocator.deallocate(blocks[35 - i]);
    }

    // We have deallocated 16 continous blocks.  Since free lists are indexed
    // with powers of two, we should be able to allocate these again.
    Block* large = allocator.allocateBlockGroup(16);

    REQUIRE(large);
    REQUIRE(1 == allocator.chunksAllocated());
}
