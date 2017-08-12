#include "internal/root-space.h"

#include "internal/memory.h"
#include "internal/page.h"
#include "internal/root.h"

#include <new>

namespace worthy {
namespace internal {

RootSpace::RootSpace() :
    first_page_{nullptr},
    free_list_head_{nullptr}
{
}

RootSpace::~RootSpace() {
    // TODO: Free all pages
    aligned_free(first_page_);
}

Root* RootSpace::newRoot(void* ptr) {
    Page* page;
    if (first_page_) {
        page = first_page_;
    } else {
        page = allocatePage();
    }

    void* memory = page->allocate(sizeof(Root));

    return new (memory) Root(page, ptr);
}

Page* RootSpace::allocatePage() {
    void* memory = aligned_alloc(PageSize, PageAlignment);
    if (!memory) {
        return nullptr;
    }

    Page* page = new (memory) Page(nullptr, PageSize);
    first_page_ = page;

    return page;
}

} } // namespace worty::internal
