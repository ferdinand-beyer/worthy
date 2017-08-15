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

    std::size_t size() const;

private:
    Map(internal::Reference* ref);

    friend class Runtime;
};


} // namespace worthy


#endif // WORTHY_MAP_H_
