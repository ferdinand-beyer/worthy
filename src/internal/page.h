#ifndef WORTHY_INTERNAL_PAGE_H_
#define WORTHY_INTERNAL_PAGE_H_


#include <boost/intrusive/list.hpp>


namespace worthy {
namespace internal {


class Object;
class Space;


class Page : public boost::intrusive::list_base_hook<> {
public:
    static const std::size_t Alignment = 2;

    static Page* pageOf(Object* obj);

protected:
    Page();

private:
    Space* const space_;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_PAGE_H_
