#include "worthy/map.h"

#include "adapters.h"
#include "internal/hashmap.h"
#include "worthy/value.h"


using worthy::internal::HashMap;
using worthy::internal::Variant;


namespace worthy {


Map::Map(HashMap* obj)
    : AbstractValue{Type::Map, obj} {
}


HashMap* Map::map() const {
    return static_cast<HashMap*>(object());
}


std::size_t Map::size() const {
    return map()->count();
}


bool Map::containsKey(const Value& key) const {
    return map()->containsKey(toVariant(key));
}


Value Map::get(const Value& key) const {
    return toValue(map()->get(toVariant(key)));
}


Map Map::add(const Value& key, const Value& value) const {
    return map()->add(toVariant(key), toVariant(value));
}


} // namespace worthy
