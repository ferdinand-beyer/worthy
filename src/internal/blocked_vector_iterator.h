#ifndef WORTHY_INTERNAL_BLOCKED_VECTOR_ITERATOR_H_
#define WORTHY_INTERNAL_BLOCKED_VECTOR_ITERATOR_H_


#include "internal/block.h"
#include "internal/check.h"

#include <boost/iterator/iterator_facade.hpp>


namespace worthy {
namespace internal {


template<typename BlockIterator, typename T>
class BlockedVectorIterator final
    : public boost::iterator_facade<
          BlockedVectorIterator<BlockIterator, T>,
          T, boost::bidirectional_traversal_tag> {
public:
    BlockedVectorIterator() : pos_{nullptr}
    {}

    BlockedVectorIterator(BlockIterator block, T* pos) :
        block_{block},
        pos_{pos}
    {}

private:
    T& dereference() const {
        return pos_ ? *pos_ : *first();
    }

    bool equal(const BlockedVectorIterator& other) const {
        return (block_ == other.block_) && (pos_ == other.pos_);
    }

    void increment() {
        if (!pos_) {
            pos_ = first();
        }
        ++pos_;
        if (pos_ > last()) {
            ++block_;
            pos_ = nullptr;
        }
    }

    void decrement() {
        if (pos_ > first()) {
            --pos_;
        } else {
            --block_;
            pos_ = last();
        }
    }

    T* first() const {
        return reinterpret_cast<T*>(block_->begin());
    }

    T* last() const {
        return reinterpret_cast<T*>(block_->current()) - 1;
    }

    BlockIterator block_;
    T* pos_;

    friend class boost::iterator_core_access;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_BLOCKED_VECTOR_ITERATOR_H_
