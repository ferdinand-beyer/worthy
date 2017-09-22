#ifndef WORTHY_INTERNAL_MAP_ITERATOR_H_
#define WORTHY_INTERNAL_MAP_ITERATOR_H_


#include "internal/iterator.h"


namespace worthy {
namespace internal {


class MapIterator : public Iterator {
public:
    Variant key() const;
    Variant value() const;

    Variant current_() const;

private:
    MapIterator() = default;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_MAP_ITERATOR_H_
