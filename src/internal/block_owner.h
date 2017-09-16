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
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_BLOCK_OWNER_H_
