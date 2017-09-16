#ifndef WORTHY_INTERNAL_BLOCK_OWNER_H_
#define WORTHY_INTERNAL_BLOCK_OWNER_H_


namespace worthy {
namespace internal {


class Block;


class BlockOwner {
public:
    virtual ~BlockOwner() = default;

protected:
    BlockOwner() = default;

    void take(Block* block);
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_BLOCK_OWNER_H_
