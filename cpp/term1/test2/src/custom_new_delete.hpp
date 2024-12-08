#include "expression.hpp"

template <typename T>
struct SingleClassMethodCounter {
    size_t new_calls = 0;
    size_t delete_calls = 0;
};

struct MethodsCounter:
    SingleClassMethodCounter<Val>,
    SingleClassMethodCounter<Add>,
    SingleClassMethodCounter<Mul>,
    SingleClassMethodCounter<Var>
{
    template<typename T>
    size_t& getNewCalls() { return static_cast<SingleClassMethodCounter<T>*>(this)->new_calls; }

    template<typename T>
    size_t& getDeleteCalls() { return static_cast<SingleClassMethodCounter<T>*>(this)->delete_calls; }
};

extern MethodsCounter* globalCounts;


struct MethodsCounterHolder {
    MethodsCounter cnt;
    MethodsCounterHolder() { globalCounts = &cnt; }
    ~MethodsCounterHolder() { globalCounts = nullptr; }
};

template <typename T>
size_t GetRegisteredNewCalls() {
    return globalCounts->getNewCalls<T>();
}
