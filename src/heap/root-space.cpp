#include "src/heap/root.h"
#include "src/heap/root-space.h"

namespace worthy {
namespace internal {

RootSpace::RootSpace(Heap* heap) : Space(heap) {
}

Address RootSpace::newRoot(Address addr) {
    // TODO
    return nullptr;
}

} // namespace internal
} // namespace worthy

