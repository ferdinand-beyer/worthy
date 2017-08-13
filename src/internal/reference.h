#ifndef WORTHY_INTERNAL_REFERENCE_H_
#define WORTHY_INTERNAL_REFERENCE_H_


#include "internal/macros.h"

#include <atomic>
#include <cstdint>


namespace worthy {
namespace internal {


class Object;


class Reference {
public:
    Reference(std::uint32_t index, void* ptr);

    bool isValid() const;

    Object* object();

    void* ptr() const;

    std::uint32_t retainCount() const;

    void retain();
    void release();

private:
    std::uint32_t pageIndex() const;
    void reset(void* ptr);

    void* ptr_;
    const std::uint32_t index_;
    std::atomic<std::uint32_t> retain_count_;

    WORTHY_DISABLE_COPY(Reference);

    friend class ReferenceSpace;
};


inline bool Reference::isValid() const {
    return retainCount() > 0;
}


inline void* Reference::ptr() const {
    return ptr_;
}


inline std::uint32_t Reference::pageIndex() const {
    return index_;
}


inline std::uint32_t Reference::retainCount() const {
    return retain_count_.load(std::memory_order_relaxed);
}


} } // namespace worty::internal


#endif // WORTHY_INTERNAL_REFERENCE_H_
