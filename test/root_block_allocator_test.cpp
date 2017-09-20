#include "internal/block_layout.h"
#include "internal/block_owner.h"
#include "internal/root_block_allocator.h"

#include <catch.hpp>

#include <set>


using namespace worthy::internal;


class TestBlockOwner : public BlockOwner {
};


TEST_CASE("Allocate single blocks", "[block]") {
    RootBlockAllocator allocator;

    SECTION("just one") {
        Block* block = allocator.allocate();

        REQUIRE(block);
        REQUIRE(allocator.blocksAllocated() == 1);
    }

    SECTION("from the same chunk") {
        std::set<Block*> blocks;

        for (int i = 0; i < BlocksPerChunk; i++) {
            Block* b = allocator.allocate();

            auto inserted = blocks.insert(b);
            REQUIRE(inserted.second);

            REQUIRE(BlockSize == b->bytesAvailable());
        }

        REQUIRE(allocator.chunksAllocated() == 1);
        REQUIRE(allocator.blocksAllocated() == BlocksPerChunk);
    }

    SECTION("more than fit in one chunk") {
        for (int i = 0; i < (BlocksPerChunk + 1); i++) {
            allocator.allocate();
        }

        REQUIRE(allocator.chunksAllocated() == 2);
        REQUIRE(allocator.blocksAllocated() == (BlocksPerChunk + 1));
    }
}


TEST_CASE("Allocate block groups", "[block]") {
    RootBlockAllocator allocator;

    SECTION("smaller than one chunk") {
        Block* block = allocator.allocate(10);

        REQUIRE(allocator.blocksAllocated() == 10);
        REQUIRE(block->bytesAvailable() == (10 * BlockSize));
    }

    SECTION("exactly one chunk") {
        Block* block = allocator.allocate(BlocksPerChunk);

        REQUIRE(allocator.chunksAllocated() == 1);
        REQUIRE(allocator.blocksAllocated() == BlocksPerChunk);
        REQUIRE(block->bytesAvailable() == (BlocksPerChunk * BlockSize));
    }

    SECTION("multiple chunks") {
        const auto nchunks = 7;
        const auto nblocks_requested = nchunks * BlocksPerChunk;

        // We will get more because consequent chunks don't have block
        // descriptors and can therefore hold more blocks.
        const auto nblocks_expected = BlocksPerChunk
            + (nchunks - 1) * (ChunkSize / BlockSize);

        Block* block = allocator.allocate(nblocks_requested);

        REQUIRE(allocator.chunksAllocated() == nchunks);
        REQUIRE(allocator.blocksAllocated() == nblocks_expected);
        REQUIRE(block->bytesAvailable() == (nblocks_expected * BlockSize));
    }
}


TEST_CASE("Deallocate blocks", "[block]") {
    RootBlockAllocator allocator;

    SECTION("within one chunk") {
        Block* first = allocator.allocate();

        // Exhaust the first chunk.
        for (int i = 1; i < BlocksPerChunk; i++) {
            allocator.allocate();
        }

        REQUIRE(allocator.chunksAllocated() == 1);
        REQUIRE(allocator.blocksAllocated() == BlocksPerChunk);

        allocator.deallocate(first);

        REQUIRE(allocator.blocksAllocated() == (BlocksPerChunk - 1));

        Block* reused = allocator.allocate();

        REQUIRE(allocator.chunksAllocated() == 1);
        REQUIRE(reused == first);
    }

    SECTION("free all blocks") {
        Block* b = allocator.allocate();
        allocator.deallocate(b);

        REQUIRE(allocator.chunksAllocated() == 1);
        REQUIRE(allocator.blocksAllocated() == 0);

        for (int i = 0; i < BlocksPerChunk; i++) {
            allocator.allocate();
        }

        REQUIRE(allocator.chunksAllocated() == 1);
    }

    SECTION("free all blocks in order") {
        Block* b1 = allocator.allocate();
        Block* b2 = allocator.allocate();

        allocator.deallocate(b1);
        allocator.deallocate(b2);

        REQUIRE(allocator.chunksAllocated() == 1);
        REQUIRE(allocator.blocksAllocated() == 0);

        for (int i = 0; i < BlocksPerChunk; i++) {
            allocator.allocate();
        }

        REQUIRE(allocator.chunksAllocated() == 1);
    }
}


TEST_CASE("Merge blocks", "[block]") {
    RootBlockAllocator allocator;

    Block* blocks[BlocksPerChunk];

    for (int i = 0; i < BlocksPerChunk; i++) {
        blocks[i] = allocator.allocate();
    }

    // Deallocate in zig-zag to trigger forward and backward merging.
    for (int i = 0; i < 8; i++) {
        allocator.deallocate(blocks[20 + i]);
        allocator.deallocate(blocks[35 - i]);
    }

    REQUIRE(allocator.blocksAllocated() == (BlocksPerChunk - 16));

    // We have deallocated 16 continous blocks.  Since free lists are indexed
    // with powers of two, we should be able to allocate these again.
    Block* large = allocator.allocate(16);

    REQUIRE(large != nullptr);
    REQUIRE(allocator.chunksAllocated() == 1);
}


TEST_CASE("Deallocate block groups", "[block]") {
    RootBlockAllocator allocator;

    Block* block = allocator.allocate(3 * BlocksPerChunk);
    allocator.deallocate(block);

    REQUIRE(allocator.chunksAllocated() == 3);
    REQUIRE(allocator.blocksAllocated() == 0);

    SECTION("reuse deallocated chunks") {
        // This should reuse space from the deallocated chunks.
        block = allocator.allocate(BlocksPerChunk);

        REQUIRE(allocator.chunksAllocated() == 3);

        // There are still two chunks available.
        Block* remaining = allocator.allocate(2 * BlocksPerChunk);

        REQUIRE(allocator.chunksAllocated() == 3);
    }

    SECTION("merge deallocated chunks") {
        Block* a = allocator.allocate(BlocksPerChunk);
        Block* b = allocator.allocate(BlocksPerChunk);
        Block* c = allocator.allocate(BlocksPerChunk);

        REQUIRE(allocator.chunksAllocated() == 3);

        allocator.deallocate(b);

        SECTION("merge in one direction") {
            allocator.deallocate(a);

            block = allocator.allocate(2 * BlocksPerChunk);
            REQUIRE(allocator.chunksAllocated() == 3);
        }

        SECTION("merge in other direction") {
            allocator.deallocate(c);

            block = allocator.allocate(2 * BlocksPerChunk);
            REQUIRE(allocator.chunksAllocated() == 3);
        }

        SECTION("merge in both direction") {
            allocator.deallocate(a);
            allocator.deallocate(c);

            block = allocator.allocate(3 * BlocksPerChunk);
            REQUIRE(allocator.chunksAllocated() == 3);
        }
    }
}


TEST_CASE("Deallocate linked list of blocks", "[block]") {
    RootBlockAllocator allocator;

    BlockList blocks;

    for (uint i = 0; i < BlocksPerChunk; i++) {
        Block* block = allocator.allocate();
        blocks.push_back(*block);
    }

    REQUIRE(allocator.chunksAllocated() == 1);

    allocator.deallocateList(blocks);

    REQUIRE(blocks.empty());

    allocator.allocate(BlocksPerChunk);

    REQUIRE(allocator.chunksAllocated() == 1);
}


TEST_CASE("Block owner is reset", "[block]") {
    TestBlockOwner owner;
    RootBlockAllocator allocator;

    Block* block = allocator.allocate();
    block->setOwner(&owner);

    REQUIRE(block->owner() == &owner);

    allocator.deallocate(block);
    Block* new_block = allocator.allocate();

    REQUIRE(new_block == block);
    REQUIRE(new_block->owner() == nullptr);
}

