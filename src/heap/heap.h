#ifndef WORTHY_HEAP_H_
#define WORTHY_HEAP_H_

#include <memory>

namespace worthy {
namespace internal {

class RootSpace;

const int PAGE_SIZE_BITS = 19;

class Heap {
public:
    Heap();
    Heap(const Heap&) = delete;
    ~Heap();

private:
    std::unique_ptr<RootSpace> root_space_;
};

} // namespace internal
} // namespace worthy

#endif // WORTHY_HEAP_H_
