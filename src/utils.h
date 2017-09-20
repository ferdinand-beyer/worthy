#ifndef WORTHY_UTILS_H_
#define WORTHY_UTILS_H_


#include "internal/heap.h"
#include "internal/object.h"


namespace worthy {


namespace internal {
class Variant;
}


class Value;


Value toValue(const internal::Variant& v);


/*!
 * \brief Guarded access to the Heap through an Object.
 */
template<typename T>
class With final {
public:
    With(const With&) = delete;
    With& operator=(const With&) = delete;

    explicit With(internal::Object* object)
            : object_{T::cast(object)}, heap_{object->heap()} {
        heap_->lock();
    }

    ~With() {
        heap_->unlock();
    }

    T* operator->() const noexcept {
        return object_;
    }

    T& operator*() const noexcept {
        return *object_;
    }

    T* get() const noexcept {
        return object_;
    }

private:
    T* const object_;
    internal::Heap* const heap_;
};


} // namespace worthy


#endif // WORTHY_UTILS_H_
