#ifndef WORTHY_INTERNAL_OBJECT_HEADER_H_
#define WORTHY_INTERNAL_OBJECT_HEADER_H_


#include "internal/globals.h"

#include <atomic>


namespace worthy {
namespace internal {


class Object;
class Page;


class ObjectHeader {
public:
    static ObjectHeader* of(Object* obj);

    Page* page() const;

private:
    std::uint16_t page_marker_;
    std::uint8_t flags_;
    std::uint8_t gc_;
    union {
        std::uint32_t size_;
        std::atomic<std::uint32_t> ref_count_;
    };

    friend class Space;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_OBJECT_HEADER_H_
