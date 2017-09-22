#include "internal/map_iterator.h"

#include "internal/hashmap.h"
#include "internal/object_dispatch.h"


namespace worthy {
namespace internal {


Variant MapIterator::key() const {
    DISPATCH_CONST(WORTHY_MAP_ITERATOR_TYPES, key_, ());
}


Variant MapIterator::value() const {
    DISPATCH_CONST(WORTHY_MAP_ITERATOR_TYPES, value_, ());
}


Variant MapIterator::current_() const {
    return key();
}


} } // namespace worthy::internal
