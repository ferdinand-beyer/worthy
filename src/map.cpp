#include "worthy/map.h"

#include "worthy/value.h"

#include "internals.h"

#include "internal/hashmap.h"


using worthy::internal::HashMap;
using worthy::internal::Reference;
using worthy::internal::Variant;


namespace worthy {


Map::Map(Reference* ref)
    : AbstractValue{Type::Map, ref} {
}


std::size_t Map::size() const {
    const HashMap* map = HashMap::cast(object());
    return map->count();
}


bool Map::containsKey(const Value& key) const {
    const HashMap* map = HashMap::cast(object());
    return map->containsKey(toVariant(key));
}


Value Map::get(const Value& key) const {
    const HashMap* map = HashMap::cast(object());
    return toValue(map->get(toVariant(key)));
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
