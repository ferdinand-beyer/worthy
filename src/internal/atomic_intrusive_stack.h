#ifndef WORTHY_INTERNAL_ATOMIC_INTRUSIVE_STACK_H_
#define WORTHY_INTERNAL_ATOMIC_INTRUSIVE_STACK_H_


#include "internal/check.h"

#include <atomic>


namespace worthy {
namespace internal {


/*!
 * \brief When declared as a \c friend, provides access to the \c next_ member
 * variable to AtomicIntrusiveStack.
 */
class IntrusiveStackAccess {
private:
    template<typename T>
    static inline T*& next(T* value) {
        return value->next_;
    }

    template<typename T>
    friend class AtomicIntrusiveStack;
};


/*!
 * \brief Simple lock-free non-owning stack.
 *
 * \tparam T Type of items in the stack.  Must declare IntrusiveStackAccess as
 * a \c friend.
 */
template<typename T>
class AtomicIntrusiveStack {
public:
    AtomicIntrusiveStack(const AtomicIntrusiveStack&) = delete;
    AtomicIntrusiveStack& operator=(const AtomicIntrusiveStack&) = delete;

    AtomicIntrusiveStack() : top_{nullptr} {
    }

    /*!
     * Puts \a value on the top of the stack.
     */
    void push(T* value) {
        WORTHY_DCHECK(value != nullptr);
        next(value) = top_.load(std::memory_order_relaxed);
        while (!top_.compare_exchange_weak(
                    next(value), value,
                    std::memory_order_release,
                    std::memory_order_relaxed));
    }

    /*!
     * Removes and returns the top-most value from the stack.  Returns
     * \c nullptr if the stack is empty.
     */
    T* pop() {
        T* value = top_.load(std::memory_order_relaxed);
        do {
            if (!value) {
                return nullptr;
            }
        } while (!top_.compare_exchange_weak(
                    value, next(value),
                    std::memory_order_release,
                    std::memory_order_relaxed));
        next(value) = nullptr;
        return value;
    }

private:
    static T*& next(T* value) {
        return IntrusiveStackAccess::next(value);
    }

    std::atomic<T*> top_;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_ATOMIC_INTRUSIVE_STACK_H_
