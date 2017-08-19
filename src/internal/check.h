#ifndef WORTHY_CHECK_INTERNAL_H_
#define WORTHY_CHECK_INTERNAL_H_


#include "internal/macros.h"

#include <boost/config.hpp>


namespace worthy {
namespace internal {


BOOST_NORETURN void fatal(const char* file, int line, const char* format, ...);


#ifdef WORTHY_DEBUG
    #define WORTHY_FILE __FILE__
    #define WORTHY_LINE __LINE__

    #ifndef WORTHY_ENABLE_DEBUG_CHECKS
        #define WORTHY_ENABLE_DEBUG_CHECKS
    #endif
#else
    #define WORTHY_FILE "(unknown)"
    #define WORTHY_LINE 0
#endif


#define WORTHY_FATAL(msg) \
    ::worthy::internal::fatal(WORTHY_FILE, WORTHY_LINE, "%s", (msg))


#define WORTHY_CHECK_MSG(condition, message)        \
    do {                                            \
        if (BOOST_UNLIKELY(!(condition))) {         \
            ::worthy::internal::fatal(              \
                    WORTHY_FILE, WORTHY_LINE,       \
                    "Check failed: %s", message);   \
        }                                           \
    } while (false)


#define WORTHY_CHECK(condition) \
    WORTHY_CHECK_MSG(condition, #condition)


#ifdef WORTHY_ENABLE_DEBUG_CHECKS


#define WORTHY_DCHECK_MSG(condition, message)           \
    do {                                                \
        if (BOOST_UNLIKELY(!(condition))) {             \
            ::worthy::internal::fatal(                  \
                    WORTHY_FILE, WORTHY_LINE,           \
                    "Debug check failed: %s", message); \
        }                                               \
    } while (false)


#define WORTHY_DCHECK(condition) \
    WORTHY_DCHECK_MSG(condition, #condition)


#else


#define WORTHY_DCHECK(condition) WORTHY_NOOP


#endif // WORTHY_ENABLE_DEBUG_CHECKS


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_CHECK_H_
