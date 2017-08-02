#ifndef WORTHY_INTERNAL_RUNTIME_H_
#define WORTHY_INTERNAL_RUNTIME_H_

#include "src/hashmap.h"
#include "src/heap/heap.h"

namespace worthy {
namespace internal {

class InternalRuntime {
public:
    InternalRuntime();

    InternalRuntime(const InternalRuntime&) = delete;

    ~InternalRuntime();

    inline const HashMap& emptyHashMap() const {
        return empty_hashmap_;
    }

private:
    Heap heap_;

    HashMap empty_hashmap_;
};

} // namespace internal
} // namespace worthy

#endif // WORTHY_INTERNAL_RUNTIME_H_

