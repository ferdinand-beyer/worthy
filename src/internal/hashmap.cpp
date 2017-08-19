#include "internal/hashmap.h"


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


Variant HashMap::nullValue() const {
    return {null_type_, null_data_};
}


const HashMap* HashMap::assoc(const Variant& key, const Variant& value) const {
    if (key.isNull()) {
        if (has_null_key_ && value == nullValue()) {
            return this;
        }
    }
    // TODO
    return this;
}


} } // namespace worthy::internal
