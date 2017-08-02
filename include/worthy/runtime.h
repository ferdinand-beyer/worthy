#ifndef WORTHY_RUNTIME_H_
#define WORTHY_RUNTIME_H_

#include <memory>

namespace worthy {

class Map;

namespace internal {
class InternalRuntime;
}

class Runtime {
public:
    Runtime();

    Runtime(const Runtime&) = delete;

    ~Runtime();

    Map map();

private:
    std::unique_ptr<internal::InternalRuntime> internal_;
};

} // namespace worthy

#endif // WORTHY_RUNTIME_H_
