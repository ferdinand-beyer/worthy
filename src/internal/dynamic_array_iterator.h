#ifndef WORTHY_INTERNAL_DYNAMIC_ARRAY_ITERATOR_H_
#define WORTHY_INTERNAL_DYNAMIC_ARRAY_ITERATOR_H_


#include <internal/block.h>
#include <internal/check.h>

#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/reverse_iterator.hpp>


namespace worthy {
namespace internal {


template<typename BlockIterator, typename T>
class DynamicArrayIterator
    : public boost::iterator_facade<
          DynamicArrayIterator<BlockIterator, T>,
          T, boost::bidirectional_traversal_tag> {
public:
    DynamicArrayIterator() : pos_{nullptr}
    {}

    DynamicArrayIterator(BlockIterator block, T* pos) :
        block_{block},
        pos_{pos}
    {}

private:
    T& dereference() const {
        return pos_ ? *pos_ : *first();
    }

    bool equal(const DynamicArrayIterator& other) const {
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


#endif // WORTHY_INTERNAL_DYNAMIC_ARRAY_ITERATOR_H_
