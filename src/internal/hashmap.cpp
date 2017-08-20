#include "internal/hashmap.h"

#include "internal/heap.h"


namespace worthy {
namespace internal {


namespace {


inline HashMap* newHashMap(const HashMap* map, ElementCount count,
        const HashMapNode* root, bool has_null_key, const Variant& null_value) {
    return map->heap()->newObject<HashMap>(count, root, has_null_key, null_value);
}


} // namespace


HashMap::HashMap()
    : Object(ObjectType::HashMap),
      root_{nullptr},
      null_value_{},
      has_null_key_{false},
      count_{0} {
}


HashMap::HashMap(ElementCount count,
                 const HashMapNode* root,
                 bool has_null_key,
                 const Variant& null_value)
    : Object{ObjectType::HashMap},
      root_{root},
      null_value_{null_value},
      has_null_key_{has_null_key},
      count_{count} {
}


bool HashMap::containsKey(const Variant& key) const {
    // TODO
    return true;
}


Variant HashMap::get(const Variant& key) const {
    // TODO
    return null_value_;
}


HashMap* HashMap::assoc(const Variant& key, const Variant& value) const {
    if (key.isNull()) {
        if (has_null_key_ && value == null_value_) {
            // No change to the map.
            return const_cast<HashMap*>(this);
        }
        return newHashMap(this, has_null_key_ ? count_ : count_ + 1,
                          root_, true, value);
    }
    // TODO
    return const_cast<HashMap*>(this);
}


} } // namespace worthy::internal
