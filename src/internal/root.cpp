#include "internal/root.h"

namespace worthy {
namespace internal {

Root::Root() : count_{0}, address_{nullptr} {
}

void Root::incRef() {
    ++count_;
}

void Root::decRef() {
    --count_;
}

} // namespace internal
} // namespace worthy
