#include "internal/root.h"
#include "internal/root-space.h"

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

