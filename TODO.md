# TODO

## Moving roots
We could try to move roots with the following idea:
- Copy the source to the target
- Save a "forwarding pointer" from the source to the target
- Whenever a root is accessed, check for the forwarding address.
  If it exists, update the containing pointer to the new address,
  releasing the reference count.

### Misc
- Transient tag: per-runtime atomic counter?

## Heap
- Make sure that the "data area" of a page is propertly aligned for all
  objects (8 or 16 bytes), by making the Page class size a multiple of
  16 or explicitly storing the data start address.
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
  *NOTE:* It might be easier to chose 8, since that's the size of
  ObjectHeader
- Store the allocated size in the Object header, because it may be
  padded for alignment.
- Store the allocated size in words, not in bytes!
- Decide what to do in 32-bit builds!

## Abstract/Final
- Check that all assignable classes are either abstract or final

## Boost
Check out:
- Boost.Interprocess for mapped_region::get_page_size()
  define: BOOST_DATE_TIME_NO_LIB
  internal/config.h?

## Arrays
- Use a base class for Arrays:

    class ArrayObject : public Object {
      VariantType type;
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

## Variant
- Reuse more of Variant in AbstractValue

