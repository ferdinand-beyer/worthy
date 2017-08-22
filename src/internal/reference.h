#ifndef WORTHY_INTERNAL_REFERENCE_H_
#define WORTHY_INTERNAL_REFERENCE_H_


#include "internal/object.h"
#include "internal/object-decl.h"


namespace worthy {
namespace internal {


class Reference : public Object {
public:
    DECL_CAST(Reference)

    explicit Reference(Object* ptr);

    Object* get();
    const Object* get() const;

    void retain();
    void release();

private:
    WORTHY_DISABLE_COPY(Reference);

    Object* ptr_;

    friend class ReferenceSpace;
};


inline Object* Reference::get() {
    return ptr_;
}


inline const Object* Reference::get() const {
    return ptr_;
}


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_REFERENCE_H_
