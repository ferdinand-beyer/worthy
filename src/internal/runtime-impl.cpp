#include "internal/runtime-impl.h"


namespace worthy {
namespace internal {


RuntimeImpl::RuntimeImpl() {
}


RuntimeImpl::~RuntimeImpl() {
}


Reference* RuntimeImpl::emptyHashMapReference() {
    // TODO: We should cache this (and other) references.
    return heap_.newReference(&empty_hashmap_);
}


} } // namespace worthy::internal
