#include "internal/gc_workspace.h"


namespace worthy {
namespace internal {


GCWorkspace::GCWorkspace(Generation* generation)
    : generation_{generation} {
}


} } // namespace worthy::internal
