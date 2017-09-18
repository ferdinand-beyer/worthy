#ifndef WORTHY_INTERNAL_HANDLE_H_
#define WORTHY_INTERNAL_HANDLE_H_


#include "internal/globals.h"

#include <boost/intrusive_ptr.hpp>

#include <atomic>


namespace worthy {
namespace internal {


class Object;


/*!
 * \brief Reference-counted handle for Object instances.
 */
class Handle final {
    struct ConstructKey {};

public:
    Handle(const Handle&) = delete;
    Handle& operator=(const Handle&) = delete;

    Handle(Object* obj, ConstructKey);

    Object* get() const;

    void retain();
    void release();

private:
    union {
        Object* obj_;
        Handle* next_;
    };
    std::atomic<uint32_t> ref_count_;

    friend class HandlePool;
    friend class IntrusiveStackAccess;
};


inline void intrusive_ptr_add_ref(Handle* h) {
    h->retain();
}


inline void intrusive_ptr_release(Handle* h) {
    h->release();
}


typedef boost::intrusive_ptr<Handle> HandlePtr;


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_HANDLE_H_
