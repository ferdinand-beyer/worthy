#ifndef WORTHY_INTERNAL_HEAP_H_
#define WORTHY_INTERNAL_HEAP_H_


#include "internal/globals.h"
#include "internal/object_space.h"

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
    Heap();

    ~Heap();

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

    HashMap* emptyHashMap() const;
    HashMapBitmapNode* emptyHashMapBitmapNode() const;

private:
    WORTHY_DISABLE_COPY(Heap);

    std::unique_ptr<ObjectSpace> object_space_;

    boost::intrusive_ptr<HashMap> empty_hash_map_;
    boost::intrusive_ptr<HashMapBitmapNode> empty_hash_map_bitmap_node_;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_HEAP_H_
