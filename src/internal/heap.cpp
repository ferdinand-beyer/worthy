#include "internal/heap.h"
#include "internal/root-space.h"

namespace worthy {
namespace internal {

Heap::Heap() {
    root_space_.reset(new RootSpace());
}

Heap::~Heap() {
}

} // namespace internal
} // namespace worthy

