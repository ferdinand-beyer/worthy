#ifndef WORTHY_MAP_H_
#define WORTHY_MAP_H_


#include "worthy/value.h"


namespace worthy {


class Map final : public Value {
public:
    std::size_t size() const;

protected:
    Map(internal::Reference* ref);

private:
    friend class Runtime;
};


} // namespace worthy


#endif // WORTHY_MAP_H_
