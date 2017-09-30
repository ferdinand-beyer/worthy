# TODO

## TODO Log
- Implement scavanging
- Support large objects
- Refactor BlockAllocators: One class BlockPool should suffice
- Support locking of BlockPool
- Move many friends to "Access" classes (Client-Attorney pattern)
- Make HandlePool thread safe, or use one HandlePool per Frame/Context
- Support multi-threaded GC workers
- Use more Block API in BlockedVector
- Use a linked list in chunks to track allocations (instead of set)
- Gather GC statistics and determine when to run GC
- Use pointers and references consistently

## Misc
- Transient tag: per-runtime atomic counter?

### Heap
- Prevent mixing values from different runtimes!
- Decide what to do in 32-bit builds!

### Arrays
- Use a base class for Arrays:

    class ArrayObject : public Object {
      VariantType type;
      uint32 size;
      Byte[] data
    };

### Inline
- Don't inline anything but the most trivial functions
- Move most of the existing inlines to their source file
- Build a performance test suite to check if inlining helps

### Debug logging
- Add debug logging to the library, for example to track allocations.
- Add warnings if Objects are not aligned nicely.

### Variant
- Reuse more of Variant in AbstractValue

