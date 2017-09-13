#ifndef WORTHY_INTERNAL_OBJECT_HEADER_H_
#define WORTHY_INTERNAL_OBJECT_HEADER_H_


#include "internal/globals.h"
#include "internal/object_type.h"

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

    void retain();
    void release();

private:
    ObjectHeader(std::uint32_t size, ObjectType type);

    std::uint32_t size_;
    PageMarker page_marker_;
    ObjectType type_;
    std::uint8_t flags_;
    std::atomic<std::uint32_t> ref_count_;
    std::uint32_t reserved_;

    friend class Space;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_OBJECT_HEADER_H_
