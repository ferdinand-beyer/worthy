#ifndef WORTHY_HEAP_ROOT_SPACE_H_
#define WORTHY_HEAP_ROOT_SPACE_H_

#include "src/heap/space.h"
#include "src/memory.h"

namespace worthy {
namespace internal {

class RootSpace : public Space {
public:
    explicit RootSpace(Heap* heap);

    Address newRoot(Address addr);
};

} // namespace internal
} // namespace worthy

#endif // WORTHY_HEAP_ROOT_SPACE_H_
