#ifndef WORTHY_INTERNAL_ROOT_H_
#define WORTHY_INTERNAL_ROOT_H_

#include "internal/macros.h"

#include <atomic>

namespace worthy {
namespace internal {

class Page;

class Root {
public:
    Root(Page* page, void* ptr);

    void* ptr() const;

    std::uint32_t refCount() const;

    void incRef();
    void decRef();

private:
    NO_COPY(Root);

    Page* page_;
    void* ptr_;
    std::atomic<std::uint32_t> count_;
};

inline void* Root::ptr() const {
    return ptr_;
}

inline std::uint32_t Root::refCount() const {
    return count_;
}

} } // namespace worty::internal

#endif // WORTHY_INTERNAL_ROOT_H_
