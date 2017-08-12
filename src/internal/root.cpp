#include "internal/root.h"

namespace worthy {
namespace internal {

Root::Root(Page* page, void* ptr) :
    page_{page},
    ptr_{ptr},
    count_{1} {
}

void Root::incRef() {
    ++count_;
}

void Root::decRef() {
    --count_;
}

} } // namespace worthy::internal
