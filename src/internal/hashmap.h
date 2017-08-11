#ifndef WORTHY_HASHMAP_H_
#define WORTHY_HASHMAP_H_

#include "worthy/value.h"

namespace worthy {
namespace internal {

class HashMap {
public:
    HashMap();
    HashMap(std::size_t count, void* root, bool has_null, ValueData null_value);

private:
    std::size_t count_;
    void* root_;
    bool has_null_;
    ValueData null_value_;
};

} // namespace internal
} // namespace worthy

#endif // WORTHY_HASHMAP_H_

