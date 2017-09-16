#ifndef WORTHY_INTERNAL_DYNAMIC_ARRAY_H_
#define WORTHY_INTERNAL_DYNAMIC_ARRAY_H_


#include <internal/block.h>
#include <internal/block_allocator.h>
#include <internal/check.h>
#include <internal/dynamic_array_iterator.h>

#include <stdexcept>


namespace worthy {
namespace internal {


template<typename T>
class DynamicArray final {
public:
    typedef T value_type;

    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    typedef value_type& reference;
    typedef const value_type& const_reference;

    typedef value_type* pointer;
    typedef const value_type* const_pointer;

    typedef DynamicArrayIterator<BlockList::iterator, value_type> iterator;
    typedef DynamicArrayIterator<BlockList::const_iterator, const value_type>
        const_iterator;

    typedef boost::reverse_iterator<iterator> reverse_iterator;
    typedef boost::reverse_iterator<const_iterator> const_reverse_iterator;

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

    void shrink_to_fit() {
        while (!blocks_.empty() && empty(blocks_.back())) {
            blocks_.pop_back_and_dispose(
                [&](auto b) { allocator_->deallocate(b); });
        }
    }

    iterator begin() noexcept {
        auto iter = blocks_.begin();
        return iterator(iter, data(*iter));
    }

    const_iterator begin() const noexcept {
        auto iter = blocks_.begin();
        return const_iterator(iter, data(*iter));
    }

    const_iterator cbegin() const noexcept {
        return begin();
    }

    iterator end() noexcept {
        return iterator(blocks_.end(), nullptr);
    }

    const_iterator end() const noexcept {
        return const_iterator(blocks_.end(), nullptr);
    }

    const_iterator cend() const noexcept {
        return end();
    }

    reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator crbegin() const noexcept {
        return rbegin();
    }

    reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crend() const noexcept {
        return rend();
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

    static size_t size(const Block& block) noexcept {
        return (block.current() - block.begin()) / ValueSize;
    }

    template<typename BlockIterator>
    static void seek(BlockIterator& block, size_type& index) {
        auto block_size = size(*block);
        while (index >= block_size) {
            index -= block_size;
            ++block;
            block_size = size(*block);
        }
    }

    reference ref(size_type index) {
        auto block = blocks_.begin();
        seek(block, index);
        return data(*block)[index];
    }

    const_reference ref(size_type index) const {
        auto block = blocks_.begin();
        seek(block, index);
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
