#ifndef WORTHY_INTERNAL_ALLOCATION_H_
#define WORTHY_INTERNAL_ALLOCATION_H_


#include "internal/globals.h"


namespace worthy {
namespace internal {


void* alignedAlloc(std::size_t size, std::size_t alignment);

void alignedFree(void* ptr);


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_ALLOCATION_H_
