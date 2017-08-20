#include "internal/hashmap.h"

#include "internal/heap.h"


namespace worthy {
namespace internal {


HashMap::HashMap()
    : Object(ObjectType::HashMap),
      has_null_key_{false},
      null_type_{VariantType::Object},
      count_{0},
      null_data_{},
      root_{nullptr} {
}


HashMap::HashMap(ElementCount count,
                 HashMapNode* root,
                 bool has_null_key,
                 const Variant& null_value)
    : Object{ObjectType::HashMap},
      has_null_key_{has_null_key},
      null_type_{null_value.type()},
      count_{count},
      null_data_{null_value.data()},
      root_{root} {
}


const HashMap* HashMap::assoc(const Variant& key, const Variant& value) const {
    if (key.isNull()) {
        if (has_null_key_ && value == nullValue()) {
            // No change to the map.
            return this;
        }
        return newHashMap(has_null_key_ ? count_ : count_ + 1, root_, true, value);
    }
    // TODO
    return this;
}


HashMap* HashMap::newHashMap(ElementCount count, HashMapNode* root,
        bool has_null_key, const Variant& null_value) const {
    return heap()->newObject<HashMap>(count, root, has_null_key, null_value);
}


Variant HashMap::nullValue() const {
    return {null_type_, null_data_};
}


} } // namespace worthy::internal
