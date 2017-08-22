#ifndef WORTHY_INTERNAL_HEAP_H_
#define WORTHY_INTERNAL_HEAP_H_


#include "internal/globals.h"
#include "internal/object-space.h"

#include <memory>


namespace worthy {
namespace internal {


class HashMap;
class HashMapBitmapNode;
class Object;
class Reference;
class ReferenceSpace;


class Heap final {
public:
    Heap();

    ~Heap();

    Reference* newReference(Object* obj);

    template<typename T, typename... Args>
    inline T* newObject(Args&&... args) {
        return object_space_->newObject<T>(std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    inline T* newDynamicObject(std::size_t extra_size, Args&&... args) {
        return object_space_->newDynamicObject<T>(
                extra_size,
                std::forward<Args>(args)...);
    }

    HashMap* emptyHashMap() const;
    HashMapBitmapNode* emptyHashMapBitmapNode() const;

    Reference* emptyHashMapReference() const;

private:
    WORTHY_DISABLE_COPY(Heap);

    std::unique_ptr<ReferenceSpace> reference_space_;
    std::unique_ptr<ObjectSpace> object_space_;

    Reference* empty_hash_map_;
    Reference* empty_hash_map_bitmap_node_;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_HEAP_H_
