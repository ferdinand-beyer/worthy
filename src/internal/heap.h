#ifndef WORTHY_INTERNAL_HEAP_H_
#define WORTHY_INTERNAL_HEAP_H_


#include "internal/macros.h"

#include <cstddef>
#include <memory>
#include <mutex>
#include <new>
#include <set>


namespace worthy {
namespace internal {


class Object;
class Reference;
class ReferenceSpace;


class Heap {
public:
    Heap();
    ~Heap();

    Reference* newReference(Object* obj);

    template<typename T, typename... Args>
    T* construct(Args&&... args) {
        void* memory = allocate(sizeof(T));
        if (!memory) {
            return nullptr;
        }
        return new (memory) T(std::forward<Args>(args)...);
    }

    void* allocate(std::size_t size);

private:
    WORTHY_DISABLE_COPY(Heap);

    std::mutex mutex_;
    std::set<void*> allocated_;
    std::unique_ptr<ReferenceSpace> reference_space_;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_HEAP_H_
