#ifndef WORTHY_INTERNAL_REFERENCE_H_
#define WORTHY_INTERNAL_REFERENCE_H_

#include "internal/macros.h"

#include <atomic>
#include <cstdint>

namespace worthy {
namespace internal {

class Reference {
public:
    Reference(std::uint32_t index, void* ptr);

    void* ptr() const;

    std::uint32_t useCount() const;
    std::uint32_t pageIndex() const;

    void use();
    void release();

private:
    void reset(void* ptr);

    void* ptr_;
    std::atomic<std::uint32_t> count_;
    const std::uint32_t index_;

    WORTHY_DISABLE_COPY(Reference);

    friend class ReferenceSpace;
};

inline void* Reference::ptr() const {
    return ptr_;
}

inline std::uint32_t Reference::useCount() const {
    return count_;
}

inline std::uint32_t Reference::pageIndex() const {
    return index_;
}

} } // namespace worty::internal

#endif // WORTHY_INTERNAL_REFERENCE_H_
