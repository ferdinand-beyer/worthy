#ifndef WORTHY_MAP_H_
#define WORTHY_MAP_H_


#include "worthy/abstract_value.h"

#include <cstddef>


namespace worthy {


namespace internal {
class HashMap;
}


class Value;


class Map final : public AbstractValue {
public:
    Map(const Map&) = default;
    Map(Map&&) = default;

    Map& operator=(const Map&) = default;
    Map& operator=(Map&&) = default;

    void swap(Map& other);

    bool isEmpty() const;

    std::size_t size() const;

    bool containsKey(const Value& key) const;

    Value get(const Value& key) const;

    Value get(const Value& key, const Value& not_found) const;

    Map add(const Value& key, const Value& value) const;

private:
    Map(internal::HashMap* obj);

    internal::HashMap* map() const;

    friend class Runtime;
};


inline void swap(Map& lhs, Map& rhs) {
    lhs.swap(rhs);
}


inline void Map::swap(Map& other) {
    AbstractValue::swap(other);
}


} // namespace worthy


#endif // WORTHY_MAP_H_
