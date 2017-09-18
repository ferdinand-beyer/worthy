#include "utils.h"

#include "internal/check.h"
#include "internal/variant.h"

#include "worthy/value.h"


using worthy::internal::Variant;
using worthy::internal::VariantType;


namespace worthy {


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
