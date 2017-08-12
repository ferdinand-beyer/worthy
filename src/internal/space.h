#ifndef WORTHY_INTERNAL_SPACE_H_
#define WORTHY_INTERNAL_SPACE_H_

#include "internal/macros.h"

#include <cstddef>
#include <utility>

namespace worthy {
namespace internal {

class Heap;

class Space {
public:
    explicit Space(Heap* heap);

    virtual ~Space();

private:
    WORTHY_DISABLE_COPY(Space);

    Heap* heap_;
};

} } // namespace worty::internal

#endif // WORTHY_INTERNAL_SPACE_H_
