#ifndef WORTHY_CHECK_INTERNAL_H_
#define WORTHY_CHECK_INTERNAL_H_


#include "internal/macros.h"

#include <boost/config.hpp>


namespace worthy {
namespace internal {


void fatal(const char* file, int line, const char* format, ...);


#if !defined(WORTHY_ENABLE_DEBUG_CHECKS)
    #if defined(DEBUG)
        #define WORTHY_ENABLE_DEBUG_CHECKS 1
    #else
        #define WORTHY_ENABLE_DEBUG_CHECKS 0
    #endif
#endif


#define WORTHY_CHECK_MSG(condition, message)                \
    do {                                                    \
        if (BOOST_UNLIKELY(!(condition))) {                 \
            ::worthy::internal::fatal(__FILE__, __LINE__,   \
                    "Check failed: %s", message);           \
        }                                                   \
    } while (false)


#define WORTHY_CHECK(condition)                             \
    WORTHY_CHECK_MSG(condition, #condition)


#if WORTHY_ENABLE_DEBUG_CHECKS


#define WORTHY_DCHECK_MSG(condition, message)               \
    do {                                                    \
        if (BOOST_UNLIKELY(!(condition))) {                 \
            ::worthy::internal::fatal(__FILE__, __LINE__,   \
                    "Debug check failed: %s", message);     \
        }                                                   \
    } while (false)


#define WORTHY_DCHECK(condition) \
    WORTHY_DCHECK_MSG(condition, #condition)


#else


#define WORTHY_DCHECK(condition) WORTHY_NOOP


#endif // WORTHY_ENABLE_DEBUG_CHECKS


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_CHECK_H_
