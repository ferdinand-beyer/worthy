#ifndef WORTHY_INTERNAL_GC_VISITOR_H_
#define WORTHY_INTERNAL_GC_VISITOR_H_


namespace worthy {
namespace internal {


class Object;


class GCVisitor {
public:
    virtual ~GCVisitor() = default;

    virtual void visit(Object*& reference) = 0;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_GC_VISITOR_H_
