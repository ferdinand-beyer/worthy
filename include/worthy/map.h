#ifndef WORTHY_MAP_H_
#define WORTHY_MAP_H_

#include "worthy/value.h"

namespace worthy {

class Map : public Value {
public:
    std::size_t size() const;
};

} // namespace worthy

#endif // WORTHY_MAP_H_
