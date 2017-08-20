#include "worthy/map.h"

#include "worthy/value.h"

#include "internals.h"

#include "internal/hashmap.h"


using worthy::internal::HashMap;


namespace worthy {


Map::Map(internal::Reference* ref)
    : AbstractValue{Type::Map, ref} {
}


std::size_t Map::size() const {
    const HashMap* map = HashMap::cast(object());
    return map->count();
}


Map Map::assoc(const Value& key, const Value& value) const {
    const HashMap* map = HashMap::cast(object());
    HashMap* result = map->assoc(toVariant(key), toVariant(value));
    if (result == map) {
        return *this;
    }
    return newReference(result);
}


} // namespace worthy
