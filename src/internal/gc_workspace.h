#ifndef WORTHY_INTERNAL_GC_WORKSPACE_H_
#define WORTHY_INTERNAL_GC_WORKSPACE_H_


#include "internal/block.h"
#include "internal/globals.h"


namespace worthy {
namespace internal {


class Generation;


class GCWorkspace {
public:
    explicit GCWorkspace(Generation* generation);

private:
    Generation* const generation_;
    BlockList done_blocks_;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_GC_WORKSPACE_H_
