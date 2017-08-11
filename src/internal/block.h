#ifndef WORTHY_HEAP_BLOCK_H_
#define WORTHY_HEAP_BLOCK_H_

#include <cstddef>

namespace worthy {
namespace internal {

class Block {
public:
    struct Structure {
        std::size_t size;
        std::size_t alignment;
    };

    virtual ~Block() = default;

    virtual Structure structure() const = 0;
};

} // namespace internal
} // namespace worthy

#endif // WORTHY_HEAP_BLOCK_H_
