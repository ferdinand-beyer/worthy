# TODO

## Reference Space
### Release Memory
We should release pages at some point.  For this, we must prioritize
pages for allocations so that low-priority pages will eventually become
empty when all of the references are released.

- Move the free list from space to page level, making sure that pages
  are isolated.
- Keep track of the number of blocks in the free list, to quickly
  determine if a page can be freed.
- The greatest challenge is the priorization heuristic.  As we prefer
  "linear" allocation on new pages for performance, we might also prefer
  to use the free list of new pages, letting old pages starve.  However,
  if the application allocates long-lived values early, old pages may
  never become empty.
- On the other hand, we expect quite short lifetimes, as any
  modification of a persistent collection leads to new roots.

The preference of using unallocated page memory over free list might not
be justified for the reference pool.  References can be pretty arbitrary
and cache locality might not be as important as for inner nodes.

Keeping the pages tightly packed is likely to benefit garbage collection
later on, as roots can be iterated faster with less fragmentation.

### Moving references
We could try to move or merge references with the following idea:
- Copy the source reference to the target reference
- Save a "forwarding pointer" from the source to the target
- Whenever a reference is accessed, check for the forwarding address.
  If it exists, update the containing pointer to the new reference,
  releasing the reference count.
- References can be merged if they contain the same pointer.

### Misc
- Transient tag: per-runtime atomic counter?

## Heap
- Make sure that the "data area" of a page is aligned to 16 bytes, by
  making the Page class size a multiple of 16 or explicitly storing the
  data start address.
- Align pages to 4KB (OS page size)
- Ideally make page sizes a multiple of the OS page size
- Use runtime limits to allow user-tuning of the heap
- Avoid default malloc/new and allocate memory for the heap in one go at
  startup.  Additional allocations should all be pages.
- The Heap class should be abstract, to allow mocks when testing spaces,
  and to experiment with different strategies.
- Allocations triggered by an Object should pass the object, so the Heap
  can determine which space to allocate in.  This allows for example for
  a separate space of internal data structures, and to optimize for
  cache locality.
- Prevent mixing values from different runtimes!
- Make sure that all Objects are 8- or 16-bit aligned.  Round all
  allocations up to 8 or 16.
- Store the allocated size in the Object header, because it may be
  padded for alignment.


## Object layout
- -8: size
- -4
- +0: type
- +4: ref-count

## Abstract/Final
- Check that all assignable classes are either abstract or final

## Boost
Check out:
- Boost.Interprocess for mapped_region::get_page_size()
  define: BOOST_DATE_TIME_NO_LIB
  internal/config.h?

## Arrays
- Use a base class for Arrays:

struct Array {
  ObjectHeader header; //< Encode type in here somehow
  uint32 size;
  Byte[] data
};

## Inline
- Don't inline anything but the most trivial functions
- Move most of the existing inlines to their source file
- Build a performance test suite to check if inlining helps

## VariantArray
- Support a fixed-size VariantArray for HashMapArrayNode?

## Debug logging
- Add debug logging to the library, for example to track allocations.
- Add warnings if Objects are not aligned nicely.

## Interfaces
- If we make sizeof(Object) zero (by externalizing the object header),
  we could use multiple inheritance for interfaces without penalty.
- But: Leaving the type in Object might be a good idea for cache lines,
  assuming that 64 bytes are read when accessing an Object through the
  pointer, we should grab as much as needed.  For normal code, this
  certainly includes the type, because we will use it for casting,
  dispatching and checks.  The header on the other hand is mainly used
  during allocation and garbage collection.
- What about the page marker?

