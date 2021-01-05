#include "var_type_dict.h"
#include <iostream>

using namespace MNN;

struct FParams : public VarTypeDict<struct A, struct B, struct Weight> 
{};

template <typename TIn>
float fun(const TIn& in) {
    auto a = in.template Get<A>();
    auto b = in.template Get<B>();
    auto weight = in.template Get<Weight>();

    return a * weight + b * (1 - weight);
}

int main()
{
    auto VarDict1 = FParams::Create() .Set<A>(1.3f) .Set<B>(2.4f) .Set<Weight>(0.1f);
    auto a1 = VarDict1.Get<A>();
    auto b1 = VarDict1.Get<B>();
    auto weight1 = VarDict1.Get<Weight>();

    auto VarDict2 = VarTypeDict<struct A, struct B, struct Weight>::Create() .Set<A>(1.3f) .Set<B>(2.4f) .Set<Weight>(0.1f);
    auto a2 = VarDict2.Get<A>();
    auto b2 = VarDict2.Get<B>();
    auto weight2 = VarDict2.Get<Weight>();
    std::cout << a1 * weight1 + b1 * (1 - weight1)<<std::endl;
    std::cout << a2 * weight2 + b2 * (1 - weight2)<<std::endl;
}