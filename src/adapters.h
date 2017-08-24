#ifndef WORTHY_ADAPTERS_H_
#define WORTHY_ADAPTERS_H_


namespace worthy {


namespace internal {
class Variant;
}


class Value;


Value toValue(const internal::Variant& v);


} // namespace worthy


#endif // WORTHY_INTERNALS_H_
