#ifndef WORTHY_INTERNAL_BLOCK_H_
#define WORTHY_INTERNAL_BLOCK_H_


#include "internal/globals.h"

#include <boost/intrusive/list.hpp>

#include <new>


namespace worthy {
namespace internal {


class BlockOwner;


static constexpr size_t ChunkBits = 20;
static constexpr size_t BlockBits = 12;
static constexpr size_t BlockDescriptorBits = 6;

static constexpr size_t ChunkSize = 1 << ChunkBits;
static constexpr size_t BlockSize = 1 << BlockBits;
static constexpr size_t BlockDescriptorSize = 1 << BlockDescriptorBits;


size_t blocksForBytes(size_t size);


/*!
 * \brief A contiguous block of memory.
 */
class Block final : public boost::intrusive::list_base_hook<> {
public:
    static constexpr uint16_t EvacuatedFlag = 0x01;
    static constexpr uint16_t ImmortalFlag = 0x02;

    Block(const Block&) = delete;
    Block& operator=(const Block&) = delete;

    /**
     * Returns the block that contains the given ptr.
     */
    static Block* of(void* ptr);

    BlockOwner* owner() const;
    void setOwner(BlockOwner* owner);

    uint16_t flags() const;
    uint16_t& flags();

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

    void* allocate(size_t size, size_t alignment);

    /**
     * Deallocate memory from the end of the block.
     */
    void deallocate(size_t size);

    template<typename T, typename... Args>
    T* construct(Args&&... args) {
        void* ptr = allocate(sizeof(T), alignof(T));
        if (!ptr) {
            throw std::bad_alloc();
        }
        return ::new (ptr) T(std::forward<Args>(args)...);
    }

private:
    explicit Block(byte* start);

    // TODO: The start address can be computed from this!
    byte* const start_;
    byte* free_;

    BlockOwner* owner_;

    uint32_t span_;
    uint16_t flags_;

    static constexpr size_t PaddingSize =
        BlockDescriptorSize
        - sizeof(boost::intrusive::list_base_hook<>)
        - sizeof(start_)
        - sizeof(free_)
        - sizeof(owner_)
        - sizeof(span_)
        - sizeof(flags_);

    byte padding_[PaddingSize];

    friend class RootBlockAllocator;
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
