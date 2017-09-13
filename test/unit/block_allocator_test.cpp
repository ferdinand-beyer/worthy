#include "internal/block_allocator.h"
#include "internal/block_layout.h"

#include <catch.hpp>

#include <set>


using namespace worthy::internal;


TEST_CASE("Allocate single blocks", "[block]") {
    BlockAllocator allocator;

    SECTION("just one") {
        Block* block = allocator.allocate();

        REQUIRE(block);
    }

    SECTION("from the same chunk") {
        std::set<Block*> blocks;

        for (int i = 0; i < BlocksPerChunk; i++) {
            Block* b = allocator.allocate();

            auto inserted = blocks.insert(b);
            REQUIRE(inserted.second);

            REQUIRE(BlockSize == b->bytesAvailable());
        }

        REQUIRE(1 == allocator.chunksAllocated());
    }

    SECTION("more than fit in one chunk") {
        for (int i = 0; i < (BlocksPerChunk + 1); i++) {
            allocator.allocate();
        }

        REQUIRE(2 == allocator.chunksAllocated());
    }
}


TEST_CASE("Allocate block groups", "[block]") {
    BlockAllocator allocator;

    SECTION("smaller than one chunk") {
        Block* block = allocator.allocate(10);

        REQUIRE((10 * BlockSize) == block->bytesAvailable());
    }

    SECTION("exactly one chunk") {
        Block* block = allocator.allocate(BlocksPerChunk);

        REQUIRE(1 == allocator.chunksAllocated());
        REQUIRE((BlocksPerChunk * BlockSize) == block->bytesAvailable());
    }

    SECTION("multiple chunks") {
        const auto nchunks = 7;
        const auto nblocks_requested = nchunks * BlocksPerChunk;

        // We will get more because consequent chunks don't have block
        // descriptors and can therefore hold more blocks.
        const auto nblocks_expected = BlocksPerChunk
            + (nchunks - 1) * (ChunkSize / BlockSize);

        Block* block = allocator.allocate(nblocks_requested);

        REQUIRE(nchunks == allocator.chunksAllocated());
        REQUIRE((nblocks_expected * BlockSize) == block->bytesAvailable());
    }
}


TEST_CASE("Deallocate blocks", "[block]") {
    BlockAllocator allocator;

    SECTION("within one chunk") {
        Block* first = allocator.allocate();

        // Exhaust the first chunk.
        for (int i = 1; i < BlocksPerChunk; i++) {
            allocator.allocate();
        }

        REQUIRE(1 == allocator.chunksAllocated());

        allocator.deallocate(first);

        Block* reused = allocator.allocate();

        REQUIRE(1 == allocator.chunksAllocated());
        REQUIRE(first == reused);
    }

    SECTION("free all blocks") {
        Block* b = allocator.allocate();
        allocator.deallocate(b);

        REQUIRE(1 == allocator.chunksAllocated());

        for (int i = 0; i < BlocksPerChunk; i++) {
            allocator.allocate();
        }

        REQUIRE(1 == allocator.chunksAllocated());
    }

    SECTION("free all blocks in order") {
        Block* b1 = allocator.allocate();
        Block* b2 = allocator.allocate();

        allocator.deallocate(b1);
        allocator.deallocate(b2);

        REQUIRE(1 == allocator.chunksAllocated());

        for (int i = 0; i < BlocksPerChunk; i++) {
            allocator.allocate();
        }

        REQUIRE(1 == allocator.chunksAllocated());
    }
}


TEST_CASE("Merge blocks", "[block]") {
    BlockAllocator allocator;

    Block* blocks[BlocksPerChunk];

    for (int i = 0; i < BlocksPerChunk; i++) {
        blocks[i] = allocator.allocate();
    }

    // Deallocate in zig-zag to trigger forward and backward merging.
    for (int i = 0; i < 8; i++) {
        allocator.deallocate(blocks[20 + i]);
        allocator.deallocate(blocks[35 - i]);
    }

    // We have deallocated 16 continous blocks.  Since free lists are indexed
    // with powers of two, we should be able to allocate these again.
    Block* large = allocator.allocate(16);

    REQUIRE(large);
    REQUIRE(1 == allocator.chunksAllocated());
}


TEST_CASE("Deallocate block groups", "[block]") {
    BlockAllocator allocator;

    Block* block = allocator.allocate(3 * BlocksPerChunk);
    REQUIRE(3 == allocator.chunksAllocated());

    allocator.deallocate(block);

    SECTION("reuse deallocated chunks") {
        // This should reuse space from the deallocated chunks.
        block = allocator.allocate(BlocksPerChunk);

        REQUIRE(3 == allocator.chunksAllocated());

        // There are still two chunks available.
        Block* remaining = allocator.allocate(2 * BlocksPerChunk);

        REQUIRE(3 == allocator.chunksAllocated());
    }

    SECTION("merge deallocated chunks") {
        Block* a = allocator.allocate(BlocksPerChunk);
        Block* b = allocator.allocate(BlocksPerChunk);
        Block* c = allocator.allocate(BlocksPerChunk);

        REQUIRE(3 == allocator.chunksAllocated());

        allocator.deallocate(b);

        SECTION("merge in one direction") {
            allocator.deallocate(a);

            block = allocator.allocate(2 * BlocksPerChunk);
            REQUIRE(3 == allocator.chunksAllocated());
        }

        SECTION("merge in other direction") {
            allocator.deallocate(c);

            block = allocator.allocate(2 * BlocksPerChunk);
            REQUIRE(3 == allocator.chunksAllocated());
        }

        SECTION("merge in both direction") {
            allocator.deallocate(a);
            allocator.deallocate(c);

            block = allocator.allocate(3 * BlocksPerChunk);
            REQUIRE(3 == allocator.chunksAllocated());
        }
    }
}

