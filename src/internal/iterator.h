#ifndef WORTHY_INTERNAL_ITERATOR_H_
#define WORTHY_INTERNAL_ITERATOR_H_


#include "internal/object.h"


namespace worthy {
namespace internal {


class Variant;


class Iterator : public Object {
public:
    Variant current() const;
    bool moveNext();

protected:
    Iterator() = default;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_ITERATOR_H_
