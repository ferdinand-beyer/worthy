#ifndef WORTHY_INTERNAL_OBJECT_VISITOR_H_
#define WORTHY_INTERNAL_OBJECT_VISITOR_H_


#include <type_traits>


namespace worthy {
namespace internal {


class Object;


/// \brief Interface for visitor of object references
///
class ObjectVisitor {
public:
    virtual ~ObjectVisitor() = default;

    template<typename T>
    inline void visit(T*& addr) {
        static_assert(std::is_base_of<Object, T>::value, "invalid type");
        doVisit(reinterpret_cast<Object*&>(addr));
    }

private:
    virtual void doVisit(Object*& addr) = 0;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_OBJECT_VISITOR_H_
