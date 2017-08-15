#include "worthy/map.h"

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


} // namespace worthy
