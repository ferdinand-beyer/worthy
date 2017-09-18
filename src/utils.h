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
 * \brief Guard access to the Heap using an object.
 */
template<typename T>
class With final {
public:
    With(const With&) = delete;
    With& operator=(const With&) = delete;

    explicit With(internal::Object* obj)
            : ptr_{T::cast(obj)}, root_{obj->heap()} {
        root_->lock();
    }

    ~With() {
        root_->unlock();
    }

    T* operator->() const noexcept {
        return ptr_;
    }

    T& operator*() const noexcept {
        return *ptr_;
    }

    T* get() const noexcept {
        return ptr_;
    }

private:
    T* const ptr_;
    internal::Heap* const root_;
};


} // namespace worthy


#endif // WORTHY_UTILS_H_
