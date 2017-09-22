#ifndef WORTHY_INTERNAL_MAP_VISITOR_H_
#define WORTHY_INTERNAL_MAP_VISITOR_H_


namespace worthy {
namespace internal {


class MapVisitor {
public:
    virtual ~MapVisitor() = default;

    virtual bool visit(const Variant& key, const Variant& value) = 0;
};


} } // namespace worthy::internal

#endif // WORTHY_INTERNAL_MAP_VISITOR_H_
