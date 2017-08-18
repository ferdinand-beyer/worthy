#ifndef WORTHY_INTERNAL_VARIANT_H_
#define WORTHY_INTERNAL_VARIANT_H_


#include "worthy/internal/variant-base.h"

#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/transform.hpp>
#include <boost/preprocessor/tuple/elem.hpp>


namespace worthy {
namespace internal {


#define WORTHY_PARATHESIZE_TUPLE_4(a, b, c, d) ((a, b, c, d))

#define WORTHY_VARIANT_TYPES \
    WORTHY_FOR_EACH_VARIANT_TYPE(WORTHY_PARATHESIZE_TUPLE_4)

#define WORTHY_VARIANT_NAME(tuple)  BOOST_PP_TUPLE_ELEM(4, 0, tuple)
#define WORTHY_VARIANT_ID(tuple)    BOOST_PP_TUPLE_ELEM(4, 1, tuple)
#define WORTHY_VARIANT_TYPE(tuple)  BOOST_PP_TUPLE_ELEM(4, 2, tuple)
#define WORTHY_VARIANT_FIELD(tuple) BOOST_PP_TUPLE_ELEM(4, 3, tuple)


enum class VariantType : std::uint8_t {
#define WORTHY_TEMP(s, _, tuple) \
    WORTHY_VARIANT_NAME(tuple) = WORTHY_VARIANT_ID(tuple)
    BOOST_PP_SEQ_ENUM( \
        BOOST_PP_SEQ_TRANSFORM(WORTHY_TEMP, ~, WORTHY_VARIANT_TYPES))
#undef WORTHY_TEMP
};


class Variant {
private:
    VariantData data_;
    VariantType type_;
};


template <typename Function>
typename Function::return_type dispatch(const VariantData& data, const VariantType& type, Function f) {
    switch (type) {
#define WORTHY_TEMP(name, id, type, field) \
    case VariantType::name: \
        return f(data.field);
    WORTHY_FOR_EACH_VARIANT_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP
    }
}


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_VARIANT_H_
