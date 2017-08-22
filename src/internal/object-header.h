#ifndef WORTHY_INTERNAL_OBJECT_HEADER_H_
#define WORTHY_INTERNAL_OBJECT_HEADER_H_


#include "internal/globals.h"
#include "internal/object-type.h"

#include <atomic>


namespace worthy {
namespace internal {


class Heap;
class Object;
class Page;
class Space;


class ObjectHeader final {
public:
    static ObjectHeader* of(const Object* obj);

    Object* object() const;

    ObjectType type() const;

    Page* page() const;
    Space* space() const;
    Heap* heap() const;

    std::uint32_t refCount() const;

    void retain();
    void release();

private:
    ObjectHeader(ObjectType type, std::uint8_t flags, std::uint32_t size);
    ObjectHeader(ObjectType type, std::uint8_t flags);

    ObjectType type_;
    std::uint8_t flags_;
    PageMarker page_marker_;
    union {
        std::uint32_t size_;
        std::atomic<std::uint32_t> ref_count_;
    };

    friend class Space;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_OBJECT_HEADER_H_
