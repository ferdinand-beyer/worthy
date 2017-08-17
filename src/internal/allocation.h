#ifndef WORTHY_INTERNAL_ALLOCATION_H_
#define WORTHY_INTERNAL_ALLOCATION_H_


#include <cstddef>


namespace worthy {
namespace internal {


void* allocateAligned(std::size_t size, std::size_t alignment);

void deallocateAligned(void* ptr);


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_ALLOCATION_H_
