#ifndef WORTHY_INTERNAL_BLOCK_H_
#define WORTHY_INTERNAL_BLOCK_H_


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

    size_t blockCount() const;

    bool isFree() const;

    void init(size_t block_count);

    byte* const start_;
    byte* free_;

    size_t block_count_;

    friend class BlockAllocator;
};


typedef boost::intrusive::list<Block> BlockList;


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_BLOCK_H_
