#ifndef WORTHY_INTERNAL_SPACE_H_
#define WORTHY_INTERNAL_SPACE_H_

#include <cstddef>
#include <utility>

#include "internal/memory.h"

namespace worthy {
namespace internal {

class Heap;

class Space {
public:
    explicit Space(Heap* heap);

    Space(const Space&) = delete;

    virtual ~Space();

private:
    Heap* heap_;
};

} } // namespace worty::internal

#endif // WORTHY_INTERNAL_SPACE_H_
