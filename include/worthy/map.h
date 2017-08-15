#ifndef WORTHY_MAP_H_
#define WORTHY_MAP_H_


#include "worthy/abstract-value.h"

#include <cstddef>


namespace worthy {


class Map final : public AbstractValue {
public:
    Map(const Map&) = default;
    Map(Map&&) = default;

    Map& operator=(const Map&) = default;
    Map& operator=(Map&&) = default;

    void swap(Map& other);

    std::size_t size() const;

private:
    Map(internal::Reference* ref);

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
