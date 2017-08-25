#ifndef WORTHY_INTERNAL_ALIGN_H_
#define WORTHY_INTERNAL_ALIGN_H_


#include "internal/globals.h"

#include <boost/align/align_down.hpp>
#include <boost/align/align_up.hpp>


namespace worthy {
namespace internal {


inline constexpr uintptr_t alignDown(uintptr_t addr, size_t alignment) {
    return boost::alignment::align_down(addr, alignment);
}


inline constexpr uintptr_t alignUp(uintptr_t addr, size_t alignment) {
    return boost::alignment::align_up(addr, alignment);
}


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_ALIGN_H_
