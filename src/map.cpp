#include "worthy/map.h"

#include "utils.h"

#include "internal/handle.h"
#include "internal/hashmap.h"

#include "worthy/value.h"


using worthy::internal::Handle;
using worthy::internal::HashMap;


namespace worthy {


Map::Map(Handle* handle) : AbstractValue{Type::Map, handle} {
    WORTHY_DCHECK(handle->get()->isHashMap());
}


Map::Map(HashMap* map) : AbstractValue{Type::Map, map} {
}


bool Map::isEmpty() const {
    return size() == 0;
}


std::size_t Map::size() const {
    With<HashMap> map(object());
    return map->count();
}


bool Map::containsKey(const Value& key) const {
    With<HashMap> map(object());
    return map->containsKey(toVariant(key));
}


Value Map::get(const Value& key) const {
    With<HashMap> map(object());
    return toValue(map->get(toVariant(key)));
}


Value Map::get(const Value& key, const Value& not_found) const {
    With<HashMap> map(object());
    return toValue(map->get(toVariant(key), toVariant(not_found)));
}


Map Map::add(const Value& key, const Value& value) const {
    With<HashMap> map(object());
    return map->add(toVariant(key), toVariant(value));
}


} // namespace worthy
