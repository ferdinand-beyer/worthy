#ifndef WORTHY_ADAPTERS_H_
#define WORTHY_ADAPTERS_H_


namespace worthy {


namespace internal {
class Object;
class Reference;
class Variant;
}


class Value;


internal::Reference* newReference(internal::Object* obj);


Value toValue(const internal::Variant& v);


} // namespace worthy


#endif // WORTHY_INTERNALS_H_
