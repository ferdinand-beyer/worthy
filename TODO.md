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

## Moving references
We could try to move or merge references with the following idea:
- Copy the source reference to the target reference
- Save a "forwarding pointer" from the source to the target
- Whenever a reference is accessed, check for the forwarding address.
  If it exists, update the containing pointer to the new reference,
  releasing the reference count.
- References can be merged if they contain the same pointer.

