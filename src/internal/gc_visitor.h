#ifndef WORTHY_INTERNAL_GC_VISITOR_H_
#define WORTHY_INTERNAL_GC_VISITOR_H_


#include <type_traits>


namespace worthy {
namespace internal {


class Object;


class GCVisitor {
public:
    virtual ~GCVisitor() = default;


    template<typename T>
    inline void visit(T*& addr) {
        static_assert(std::is_base_of<Object, T>::value, "invalid address");
        doVisit(reinterpret_cast<Object*&>(addr));
    }

private:
    virtual void doVisit(Object*& addr) = 0;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_GC_VISITOR_H_
