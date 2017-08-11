#ifndef WORTHY_HEAP_ROOT_H_
#define WORTHY_HEAP_ROOT_H_

#include "internal/memory.h"

#include <atomic>

namespace worthy {
namespace internal {

class Root {
public:
    Root();

    inline Address address() const {
        return address_;
    }

    void incRef();
    void decRef();

private:
    std::atomic_size_t count_;
    Address address_;
};

} // namespace internal
} // namespace worthy

#endif // WORTHY_HEAP_ROOT_H_
