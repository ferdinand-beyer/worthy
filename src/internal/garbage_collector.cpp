#include "internal/garbage_collector.h"


namespace worthy {
namespace internal {


// Forwarding pointers: We could reorder the fields in Object
// and make the flags_ field first, then use the LSB to
// distinguish live objects from forwarding pointers.  This
// works as objects are 8-byte aligned, so the last three bits
// of object pointers (forwarding addresses) are always zero.
// Could be an 'Alive' bit.
// XXX: For parallel GC, we need an atomic operation.


GarbageCollector::GarbageCollector() {
    // Initialize workspace per Generation
    // Needs:
    // - Generations
}


void GarbageCollector::collect() {
    // Take: (Max) Generation to collect

    // TODO: Workers?
    
    // Prepare gens:
    // - Stash blocks (to-space -> from-space)
    // - Flag blocks as not evacuated (from-space)
    // Prepare older gens: stash remembered sets

    // Scavenge old remembered sets

    // Evacuate all roots

    // Scavenge until there is nothing more to do.

    // Clean-up gens:
    // - Free old blocks (from-space)
    // Clean-up older gens:
    // - Record evacuated large objects

    // Reset nurseries
}


} } // namespace worthy::internal
