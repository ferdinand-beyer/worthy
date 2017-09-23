#ifndef WORTHY_RUNTIME_H_
#define WORTHY_RUNTIME_H_


#include <memory>


namespace worthy {


namespace internal {
class Heap;
}


class Map;


class Runtime final {
public:
    Runtime();
    ~Runtime();

    Map map();

private:
    std::unique_ptr<internal::Heap> heap_;
};


} // namespace worthy


#endif // WORTHY_RUNTIME_H_
