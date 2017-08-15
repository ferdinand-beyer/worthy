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

## Misc
- Transient tag: per-runtime atomic counter?

### Generalized heap
- Make Reference an Object
- Currently it has: Pointer, Index (32), RefCount (32)
- It needs: Type (8), GC info

New object types:
- Reference
- FixedSizeFreeSpace: Page knows the block size

Strategies to access an Object's page:
- Fixed-block Space: block index
- Fixed-block Space: control block w/ page number, close to next blocks,
  to use a compact pointer within the data block
- Aligned pages, page pointer can be inferred from block pointer

Strategies to implement Garbage Collection:
- Reference counting (atomic counter, 32)
- Tracing: Space requirement TBD, external bitmap?

Common patterns:
- Allocate pages
- Segment allocated space into blocks

Desirable:
- For fixed-block-size spaces, the size of the page header and the size
  of blocks should fit tightly into a page
- Use reserved space in an object for different GC strategies
- Strategy how to access page must be inferable from the Object header

Object header: Some union of:
- Type: 8
- Forwarding Address (Pointer): 64
- RefCount: 32
- Block Index: 32 -- Required size depends on page and block size.  A
  16-bit index can address 64k (2^16) blocks.  With a minimum block size
  of 128 bits (Object header), the maximum page size would be 8M (128 *
  64k = 2^7 * 2^16 = 2^23)


struct ObjectHeader {
  uint8 type
  uint8 flags // static? pinned? moved? 16-bit aligned?
  uint16 index
  uint32 reserved32; // refcount? age?
  uint64 reserved64; // pointer? size?
};

- We could also strip the reserved64 and just make sure that objects are
  at least 16 bytes long, reusing 8 bytes of the payload for forwarding
  addresses.
- "Special" objects could use the object header, e.g. free space,
  references, arrays.

See also:
https://stackoverflow.com/questions/26357186/what-is-in-java-object-header

## Arrays
- Use a base class for Arrays:

struct Array {
  ObjectHeader header; //< Encode type in here somehow
  uint32 size;
  Byte[] data
};


## Style
- consider using underscore_separated_words for all methods.  Pro:
  readable.  Con: Need to change style for things like isHashMap,
  generated using a Macro.  is_hash_map.
- consider using the same naming for variables and constants.  Pro:
  there are const fields that are not necessarily variables.  Con: Use a
  special name for compile-time constants.  Con: Without uppercasing,
  cannot use predefined names as constants (Null, Bool, Double).
- consider pulling the cstdint types into the worthy::internal
  namespace, or typedef'ing them

