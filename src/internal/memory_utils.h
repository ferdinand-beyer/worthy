///
/// \file
/// \brief Memory utilities
///
#ifndef WORTHY_INTERNAL_MEMORY_UTILS_H_
#define WORTHY_INTERNAL_MEMORY_UTILS_H_


#include "globals.h"

#include <type_traits>

#include <boost/align/align_up.hpp>


namespace worthy {
namespace internal {


/// Calls the constructor of the object pointed to.
/// \sa std::destroy_at (C++17)
template<typename T>
inline void destroy(T* ptr) {
    ptr->~T();
}


class SizeCalculator final {
public:
    SizeCalculator() : size_{0} {}

    size_t size() const {
        return size_;
    }

    template<typename T>
    SizeCalculator& add() {
        size_ = boost::alignment::align_up(size_, alignof(T)) + sizeof(T);
        return *this;
    }

    template<typename T>
    SizeCalculator& add(size_t count) {
        size_ = boost::alignment::align_up(size_, alignof(T)) +
            count * sizeof(T);
        return *this;
    }
private:
    size_t size_;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_MEMORY_UTILS_H_
