Features:

- Immutable
- Persistent collections
- Rich palette of common primitive values
- Thread-Safe
- Garbage Collection

Inspiration:

- Clojure
- FlatBuffers (direct-access memory)
- SQLite (for allocator)

Types:

- Primitive
  - Nil
  - Boolean
  - Number
    - Integer: unsigned, size TBD
    - Real/Float: float/double TBD
  - Tagged type: Symbol + Value, for extension
- Indirect
  - String (Unicode)
  - Bytes
  - Symbol: Interned, Namespace-Support
  - Collection
    - List: Linked TBD
    - Map
    - Set

Ideas:

- Persistent data structures can be implemented by a growing buffer,
  just add nodes and a new root
- Use reference counting for references given to the outside
- Can use a garbage collector for internal data

Memory:

- Interned symbols
- Strings (can be shared)
- Bytes (can be shared)
- Nodes of collections
- References in user-space

API:

```
template<class... Types> List list(Types... args);
template<class... Types> Map map(Types... args);
template<class... Types> Set set(Types... args);

String str(const char* s);

Symbol symbol(const char* name);
Symbol symbol(const char* ns, const char* name);
```

Memory:

- Pages
- Spaces consisting of pages
- Pointers: Page number + offset
- 


Strategy:

- Symbols: 

-------------------------------------------------------------------------------

Heap block:
- Size (useful to traverse blocks in GC)
- GC flags (mark-sweep, lock)

Root object (public references):
- Fixed-size
- Memory address
- Reference count
- Free list (no fragmentation)

Node space:
- For map/sets, vectors and linked lists
- Limited number of sizes
- Free list per size

Internal value:
- Tagged union
- Space for pointer or primitive

TBD: Fixed-type collections?
- Save space

-------------------------------------------------------------------------------

Typical sizes:

- Cache line: 64 bytes
- OS page: 4096 bytes


