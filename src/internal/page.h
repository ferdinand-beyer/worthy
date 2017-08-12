#ifndef WORTHY_INTERNAL_PAGE_H_
#define WORTHY_INTERNAL_PAGE_H_

#include <atomic>

namespace worthy {
namespace internal {

class Space;

class Page {
public:
    Page(Space* owner, std::size_t size);

    void* allocate(std::size_t size);

private:
    std::size_t size_;
    Space* space_;

    void* data_;
};

} // namespace internal
} // namespace worthy

#endif // WORTHY_INTERNAL_PAGE_H_
