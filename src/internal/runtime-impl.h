#ifndef WORTHY_INTERNAL_RUNTIME_IMPL_H_
#define WORTHY_INTERNAL_RUNTIME_IMPL_H_


#include "internal/hashmap.h"
#include "internal/heap.h"
#include "internal/macros.h"


namespace worthy {
namespace internal {


class RuntimeImpl {
public:
    RuntimeImpl();
    ~RuntimeImpl();

    inline Heap* heap() {
        return &heap_;
    }

    inline HashMap* emptyHashMap() const {
        return const_cast<HashMap*>(&empty_hashmap_);
    }

    Reference* emptyHashMapReference();

private:
    WORTHY_DISABLE_COPY(RuntimeImpl);

    Heap heap_;
    HashMap empty_hashmap_;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_RUNTIME_IMPL_H_
