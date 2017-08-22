#include "adapters.h"

#include "internal/check.h"
#include "internal/heap.h"
#include "internal/object.h"
#include "internal/reference.h"
#include "internal/variant.h"
#include "worthy/value.h"


using worthy::internal::Object;
using worthy::internal::Reference;
using worthy::internal::Variant;
using worthy::internal::VariantType;


namespace worthy {


Reference* newReference(Object* obj) {
    return obj->heap()->newReference(obj);
}


Value toValue(const Variant& v) {
    switch (v.type()) {
    case VariantType::Object:
        // TODO
        return Value();

#define WORTHY_TEMP(name, id, type, field)  \
    case VariantType::name:                 \
        return Value(v.data().field);
    WORTHY_FOR_EACH_PRIMITIVE_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP

    default:
        WORTHY_UNREACHABLE();
    }
}


} // namespace worthy
