#ifndef WORTHY_INTERNAL_BLOCK_H_
#define WORTHY_INTERNAL_BLOCK_H_


#include "internal/block-layout.h"
#include "internal/globals.h"

#include <boost/intrusive/list.hpp>


namespace worthy {
namespace internal {


class Block : public boost::intrusive::list_base_hook<> {
public:
    /**
     * Returns the block that contains the given ptr.
     */
    static Block* of(void* ptr);

    byte* begin() const;
    byte* end() const;

    /**
     * Returns the number of free bytes in this block.
     */
    size_t bytesAvailable() const;

private:
    explicit Block(byte* start);

    bool isFree() const;
    void setFree();

    void init();

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
};


typedef boost::intrusive::list<Block> BlockList;


static_assert(sizeof(Block) == BlockDescriptorSize,
              "invalid block descriptor size");


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_BLOCK_H_
