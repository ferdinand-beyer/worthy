#include "internal/iterator.h"

#include "internal/hashmap.h"
#include "internal/object_dispatch.h"
#include "internal/variant.h"


namespace worthy {
namespace internal {


Variant Iterator::current() const {
    DISPATCH_CONST(WORTHY_ITERATOR_TYPES, current_, ());
}


bool Iterator::moveNext() {
    DISPATCH(WORTHY_ITERATOR_TYPES, moveNext_, ());
}


} } // namespace worthy::internal
