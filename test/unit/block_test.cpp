#include "internal/block.h"
#include "internal/block_layout.h"

#include <catch.hpp>


using namespace worthy::internal;


namespace {


constexpr uintptr_t chunkAddress(int chunk_no) {
    return chunk_no * ChunkSize;
}


constexpr uintptr_t descriptorAddress(int chunk_no, int block_no) {
    return chunkAddress(chunk_no) + BlockDescriptorOffset
        + (block_no * BlockDescriptorSize);
}


constexpr uintptr_t blockAddress(int chunk_no, int block_no) {
    return chunkAddress(chunk_no) + BlockOffset
        + (block_no * BlockSize);
}


} // namespace


TEST_CASE("Block addresses", "[block]") {
    constexpr uintptr_t descrAddr = descriptorAddress(1337, 123);
    constexpr uintptr_t objAddr = blockAddress(1337, 123) + (42 * 32);

    Block* block = Block::of(reinterpret_cast<void*>(objAddr));

    REQUIRE(descrAddr == reinterpret_cast<uintptr_t>(block));
}
