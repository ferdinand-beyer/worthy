#ifndef WORTHY_INTERNAL_BLOCK_H_
#define WORTHY_INTERNAL_BLOCK_H_


#include "internal/globals.h"

#include <boost/intrusive/list.hpp>


namespace worthy {
namespace internal {


class Block : public boost::intrusive::list_base_hook<> {
public:
    /**
     * Return the block that contains the given ptr.
     */
    static Block* of(void* ptr);

private:
    explicit Block(byte* start);

    byte* const start_;
    byte* free_;

    friend class BlockAllocator;
};


typedef boost::intrusive::list<Block> BlockList;


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_BLOCK_H_
