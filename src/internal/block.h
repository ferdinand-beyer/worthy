#ifndef WORTHY_INTERNAL_BLOCK_H_
#define WORTHY_INTERNAL_BLOCK_H_


#include "internal/globals.h"

#include <boost/intrusive/list.hpp>


namespace worthy {
namespace internal {


static constexpr size_t ChunkBits = 20;
static constexpr size_t BlockBits = 12;
static constexpr size_t BlockDescriptorBits = 6;

static constexpr size_t ChunkSize = 1 << ChunkBits;
static constexpr size_t BlockSize = 1 << BlockBits;
static constexpr size_t BlockDescriptorSize = 1 << BlockDescriptorBits;


class Block final : public boost::intrusive::list_base_hook<> {
public:
    /**
     * Returns the block that contains the given ptr.
     */
    static Block* of(void* ptr);

    WORTHY_DISABLE_COPY(Block);

    byte* begin() const;
    byte* current() const;
    byte* end() const;

    size_t capacity() const;
    size_t bytesAllocated() const;

    /**
     * Returns the number of free bytes in this block.
     */
    size_t bytesAvailable() const;

    /**
     * Allocate memory (without alignment requirements).
     */
    void* allocate(size_t size);

    /**
     * Deallocate memory from the end of the block.
     */
    void deallocate(size_t size);

private:
    explicit Block(byte* start);

    byte* const start_;
    byte* free_;

    size_t block_count_;

    static constexpr size_t PaddingSize =
        BlockDescriptorSize
        - sizeof(boost::intrusive::list_base_hook<>)
        - sizeof(start_)
        - sizeof(free_)
        - sizeof(block_count_);

    byte padding_[PaddingSize];

    friend class BlockAllocator;
    friend class BlockTestAccess;
};


static_assert(sizeof(Block) == BlockDescriptorSize,
              "invalid block descriptor size");


typedef boost::intrusive::list<Block> BlockList;


class BlockTestAccess {
public:
    static constexpr size_t MinBufferSize = 2 * ChunkSize;

    static Block* construct(byte* buffer);
};



} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_BLOCK_H_
