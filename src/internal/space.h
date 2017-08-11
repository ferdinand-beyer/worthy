#ifndef WORTHY_HEAP_SPACE_H_
#define WORTHY_HEAP_SPACE_H_

#include <cstddef>
#include <utility>

#include "internal/memory.h"

namespace worthy {
namespace internal {

class Heap;
class Page;

class Space {
public:
    explicit Space(Heap* heap);

    Space(const Space&) = delete;

    virtual ~Space();

    template<typename T, typename... Args>
    T* constructBlock(Args&&... args) {
        Address addr = allocateBlock(sizeof(T), alignof(T));
        if (!addr) {
            return nullptr;
        }
        return new(addr) T(std::forward<Args>(args)...);
    }

protected:
    virtual Address allocateBlock(std::size_t size, std::size_t alignment);

    Page* allocatePage(std::size_t size);

private:
    Heap* heap_;
    Page* first_page_;
};

} // namespace internal
} // namespace worthy

#endif // WORTHY_HEAP_SPACE_H_
