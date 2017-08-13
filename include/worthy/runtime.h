#ifndef WORTHY_RUNTIME_H_
#define WORTHY_RUNTIME_H_


#include <memory>


namespace worthy {
namespace internal {

class RuntimeImpl;

} // namespace internal


class Map;


class Runtime {
public:
    Runtime();

    ~Runtime();

    Map map();

private:
    std::unique_ptr<internal::RuntimeImpl> rt_;
};


} // namespace worthy


#endif // WORTHY_RUNTIME_H_
