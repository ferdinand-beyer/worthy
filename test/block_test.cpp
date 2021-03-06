#include "internal/block.h"
#include "internal/block_layout.h"

#include <catch.hpp>

#include <cstring>


using namespace worthy::internal;


namespace {


constexpr size_t TestBufferSize = BlockTestAccess::MinBufferSize;


Block* initBlock(byte* buffer) {
    std::memset(buffer, 0, TestBufferSize);
    return BlockTestAccess::construct(buffer);
}


} // namespace


TEST_CASE("Block addresses", "[block]") {
    byte* chunk = reinterpret_cast<byte*>(1337 * ChunkSize);
    byte* descr = chunk + FirstBlockDescriptorOffset
        + 10 * BlockDescriptorSize;
    byte* block = chunk + FirstBlockOffset + 10 * BlockSize;
    void* ptr = block + 123;

    REQUIRE(Block::of(ptr) == reinterpret_cast<Block*>(descr));
}


TEST_CASE("Block flags", "[block]") {
    byte buffer[TestBufferSize];
    Block* block = initBlock(buffer);

    SECTION("are initially clear") {
        REQUIRE(block->flags() == 0);
    }

    SECTION("can be set") {
        block->setFlags(Block::Eternal);
        REQUIRE(block->hasFlags(Block::Eternal));
    }
}


TEST_CASE("Allocating from block", "[block]") {
    byte buffer[TestBufferSize];
    Block* block = initBlock(buffer);

    SECTION("returns valid pointer when space available") {
        void* result = block->allocate(10);
        REQUIRE(result == block->begin());
    }

    SECTION("returns null pointer when no space available") {
        void* result = block->allocate(2 * BlockSize);
        REQUIRE(result == nullptr);
    }

    SECTION("decreases available space") {
        block->allocate(10);
        REQUIRE(block->bytesAvailable() == (BlockSize - 10));
    }

    SECTION("works for block size") {
        void* result = block->allocate(BlockSize);
        REQUIRE(result == block->begin());
        REQUIRE(block->bytesAvailable() == 0);
    }

    SECTION("can be deallocated") {
        block->allocate(BlockSize);
        block->deallocate(10);
        REQUIRE(block->bytesAvailable() == 10);
    }
}

