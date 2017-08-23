#include "internal/object-header.h"

#include "internal/page.h"
#include "internal/space.h"


namespace worthy {
namespace internal {


ObjectHeader* ObjectHeader::of(const Object* obj) {
    return reinterpret_cast<ObjectHeader*>(const_cast<Object*>(obj)) - 1;
}


ObjectHeader::ObjectHeader(ObjectType type,
                           std::uint8_t flags,
                           std::uint32_t size)
    : type_{type},
      flags_{flags},
      size_{size} {
}


ObjectHeader::ObjectHeader(ObjectType type, std::uint8_t flags)
    : type_{type},
      flags_{flags},
      ref_count_{1} {
}


Object* ObjectHeader::object() const {
    return reinterpret_cast<Object*>(const_cast<ObjectHeader*>(this) + 1);
}


ObjectType ObjectHeader::type() const {
    return type_;
}


Page* ObjectHeader::page() const {
    return Page::fromMarker(&page_marker_);
}


Space* ObjectHeader::space() const {
    return page()->space();
}


Heap* ObjectHeader::heap() const {
    return space()->heap();
}


std::uint32_t ObjectHeader::refCount() const {
    return ref_count_.load(std::memory_order_relaxed);
}


void ObjectHeader::retain() {
    ref_count_.fetch_add(1, std::memory_order_relaxed);
}


void ObjectHeader::release() {
    if (ref_count_.fetch_sub(1, std::memory_order_release) == 1) {
        std::atomic_thread_fence(std::memory_order_acquire);
        SpaceReclaimAccess::reclaim(space(), object());
    }
}


} } // namespace worthy::internal
