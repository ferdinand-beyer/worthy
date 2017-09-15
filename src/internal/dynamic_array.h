#ifndef WORTHY_INTERNAL_DYNAMIC_ARRAY_H_
#define WORTHY_INTERNAL_DYNAMIC_ARRAY_H_


#include <internal/block.h>
#include <internal/block_allocator.h>
#include <internal/check.h>

#include <stdexcept>


namespace worthy {
namespace internal {


template<class T>
class DynamicArray final {
public:
    typedef T value_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;

    DynamicArray(const DynamicArray&) = delete;
    DynamicArray& operator=(const DynamicArray&) = delete;

    explicit DynamicArray(BlockAllocator* allocator) noexcept :
        allocator_{allocator},
        size_{0}
    {
        WORTHY_CHECK(allocator_);
    }

    ~DynamicArray() noexcept {
        allocator_->deallocate(blocks_);
    }

    bool empty() const noexcept {
        return size_ == 0;
    }

    size_type size() const noexcept {
        return size_;
    }

    size_type capacity() const noexcept {
        return blocks_.size() * ValuesPerBlock;
    }

    void reserve(size_type new_capacity) {
        size_type cap = capacity();
        while (cap < new_capacity) {
            allocateBlock();
            cap += ValuesPerBlock;
        }
    }

    void shrink_to_fit() {
        auto block = blocks_.end();
        const auto end = block--;
        while (block != end && empty(*block)) {
            allocator_->deallocate(&*block);
            block = blocks_.erase(block);
        }
    }

    reference at(size_type pos) {
        if (pos < size_) {
            return ref(pos);
        }
        throw std::out_of_range("invalid index");
    }

    const_reference at(size_type pos) const {
        if (pos < size_) {
            return ref(pos);
        }
        throw std::out_of_range("invalid index");
    }

    reference operator[](size_type pos) {
        return ref(pos);
    }

    const_reference operator[](size_type pos) const {
        return ref(pos);
    }

    reference front() {
        WORTHY_CHECK(size_ > 0);
        return *data(blocks_.front());
    }

    const_reference front() const {
        WORTHY_CHECK(size_ > 0);
        return *data(blocks_.front());
    }

    reference back() {
        WORTHY_CHECK(size_ > 0);
        return ref(size_ - 1);
    }

    const_reference back() const {
        WORTHY_CHECK(size_ > 0);
        return ref(size_ - 1);
    }

    void clear() noexcept {
        while (size_ > 0) {
            pop_back();
        }
    }

    void push_back(const_reference value) {
        ::new (allocate()) value_type(value);
    }

    void push_back(value_type&& value) {
        ::new (allocate()) value_type(std::move(value));
    }

    template<typename... Args>
    reference emplace_back(Args&&... args) {
        return *(::new (allocate()) value_type(std::forward<Args>(args)...));
    }

    void pop_back() {
        WORTHY_CHECK(size_ > 0);
        auto block = blocks_.rbegin();
        while (empty(*block)) {
            ++block;
            WORTHY_DCHECK(block != blocks_.rend());
        }
        pointer ptr = reinterpret_cast<pointer>(block->current()) - 1;
        ptr->~value_type();
        block->deallocate(ValueSize);
        --size_;
    }

private:
    static constexpr size_t ValueSize = sizeof(value_type);
    static constexpr size_t ValuesPerBlock = BlockSize / ValueSize;

    static pointer data(Block& block) noexcept {
        return reinterpret_cast<pointer>(block.begin());
    }

    static const_pointer data(const Block& block) noexcept {
        return reinterpret_cast<const_pointer>(block.begin());
    }

    static bool empty(const Block& block) noexcept {
        return block.current() == block.begin();
    }

    reference ref(size_type index) {
        auto block = blocks_.begin();
        while (index >= ValuesPerBlock) {
            index -= ValuesPerBlock;
            ++block;
        }
        return data(*block)[index];
    }

    const_reference ref(size_type index) const {
        auto block = blocks_.begin();
        while (index >= ValuesPerBlock) {
            index -= ValuesPerBlock;
            ++block;
        }
        return data(*block)[index];
    }

    void* allocate() {
        void* memory = freeBlock().allocate(ValueSize);
        ++size_;
        return memory;
    }

    Block& freeBlock() {
        if (!blocks_.empty()) {
            Block& block = blocks_.back();
            if (block.bytesAvailable() >= ValueSize) {
                return block;
            }
        }
        return allocateBlock();
    }

    Block& allocateBlock() {
        Block* block = allocator_->allocate(1);
        blocks_.push_back(*block);
        return *block;
    }

    BlockAllocator* const allocator_;
    BlockList blocks_;
    size_type size_;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_DYNAMIC_ARRAY_H_
