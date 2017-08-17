#ifndef WORTHY_INTERNAL_REFERENCE_H_
#define WORTHY_INTERNAL_REFERENCE_H_


#include "internal/macros.h"
#include "internal/object.h"

// Has to be the last include (doesn't have include guards)
#include "internal/object-macros.h"


namespace worthy {
namespace internal {


class ReferenceSpace;


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

    ReferenceSpace* space();

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


#define UNDEF_OBJECT_MACROS
#include "internal/object-macros.h"


#endif // WORTHY_INTERNAL_REFERENCE_H_
