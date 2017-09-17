#ifndef WORTHY_INTERNAL_HEAP_H_
#define WORTHY_INTERNAL_HEAP_H_


#include "internal/globals.h"
#include "internal/handle_container.h"
#include "internal/object_space.h"
#include "internal/root_block_allocator.h"

#include <boost/intrusive_ptr.hpp>

#include <memory>
#include <new>


namespace worthy {
namespace internal {


class HashMap;
class HashMapBitmapNode;
class Object;


class Heap final {
public:
    Heap(const Heap&) = delete;
    Heap& operator=(const Heap&) = delete;

    Heap();
    ~Heap();

    HandlePtr makeHandle(Object* obj);

    template<typename T, typename... Args>
    inline T* make(Args&&... args) {
        void* memory = object_space_->allocate<T>();
        return new (memory) T(std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    inline T* makeExtra(std::size_t extra_size, Args&&... args) {
        void* memory = object_space_->allocateExtra<T>(extra_size);
        return new (memory) T(std::forward<Args>(args)...);
    }

    HandlePtr emptyHashMapHandle() const;
    HandlePtr emptyHashMapBitmapNodeHandle() const;

    HashMap* emptyHashMap() const;
    HashMapBitmapNode* emptyHashMapBitmapNode() const;

private:
    RootBlockAllocator allocator_;

    // TODO: Per-thread
    HandleContainer handles_;

    // TODO: Obsolete
    std::unique_ptr<ObjectSpace> object_space_;

    HandlePtr empty_hashmap_;
    HandlePtr empty_hashmap_bitmap_node_;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_HEAP_H_
