#ifndef WORTHY_INTERNAL_GARBAGE_COLLECTOR_
#define WORTHY_INTERNAL_GARBAGE_COLLECTOR_


namespace worthy {
namespace internal {


class GarbageCollector final {
public:
    GarbageCollector(const GarbageCollector&) = delete;
    GarbageCollector& operator=(const GarbageCollector&) = delete;

    GarbageCollector();

    void collect();
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_GARBAGE_COLLECTOR_
