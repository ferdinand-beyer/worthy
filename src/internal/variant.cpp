#include "internal/variant.h"

#include "internal/hash.h"
#include "internal/objects.h"


namespace worthy {
namespace internal {


HashCode hash(const Variant& variant) {
    switch (variant.type()) {
#define WORTHY_TEMP(name, id, type, field)  \
    case VariantType::name:                 \
        return hash(variant.data().field);
    WORTHY_FOR_EACH_VARIANT_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP
    }
    WORTHY_UNREACHABLE();
}


bool Variant::operator==(const Variant& other) const {
    if (type_ == other.type_) {
        switch (type_) {
#define WORTHY_TEMP(name, id, type, field)              \
        case VariantType::name:                         \
            return data_.field == other.data_.field;
        WORTHY_FOR_EACH_PRIMITIVE_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP
        case VariantType::Object:
            return Object::equals(data_.obj, other.data_.obj);
        default:
            WORTHY_UNREACHABLE();
        }
    }
    return false;
}


} } // namespace worthy::internal
