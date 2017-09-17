#include "worthy/map.h"

#include "adapters.h"
#include "internal/handle.h"
#include "internal/hashmap.h"
#include "worthy/value.h"


using worthy::internal::Handle;
using worthy::internal::HashMap;
using worthy::internal::Variant;


namespace worthy {


Map::Map(Handle* handle)
    : AbstractValue{Type::Map, handle} {
    WORTHY_DCHECK(handle->get()->isHashMap());
}


Map::Map(HashMap* map)
    : AbstractValue{Type::Map, map} {
}


HashMap* Map::map() const {
    return static_cast<HashMap*>(object());
}


bool Map::isEmpty() const {
    return size() == 0;
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


Value Map::get(const Value& key, const Value& not_found) const {
    return toValue(map()->get(toVariant(key), toVariant(not_found)));
}


Map Map::add(const Value& key, const Value& value) const {
    return map()->add(toVariant(key), toVariant(value));
}


} // namespace worthy
