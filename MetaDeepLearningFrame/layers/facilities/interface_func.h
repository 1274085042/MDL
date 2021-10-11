#pragma once

#include <ostream>
#include <type_traits>

namespace MDL
{
    template<typename tLayer, typename tIn>
    auto LayerFeedForward(tLayer &layer, tIn && in)
    {
        return layer.FeedForward(forward<tIn>(in));
    }

    template<typename tLayer, typename tGrad>
    auto LayerFeedBackward(tLayer &layer, tGrad && grad)
    {
        return layer.FeedBackward(forward<tGrad>(grad));
    }

    /*
    * decltype()
    * 如果参数是函数，那么函数带参数和不带参数的返回类型是不一样的
    * - 不带参数，返回函数指针类型
    * - 带参数，返回函数返回类型
    */
    template<typename L, typename tInitializer, typename tBuffer, typename tInitPolicies>
    std::true_type InitTest(decltype(&L::template Init<tInitializer, tBuffer, tInitPolicies>));
    template<typename L, typename tInitializer, typename tBuffer, typename tInitPolicies>
    std::false_type InitTest(...);
    template<typename tLayer, typename tInitializer, typename tBuffer, typename tInitPolicies = typename tInitializer::PolicyCont>
    void LayerInit(tLayer &layer, tInitializer &initialzier, tBuffer &loadBuffer, std::ostream *log = nullptr)
    {
        if constexpr(decltype(InitTest<tLayer, tInitializer, tBuffer,tInitPolicies>(nullptr))::value)
        {
            layer.template Init<tInitializer, tBuffer, tInitPolicies>(initialzier, loadBuffer, log);
        }
    }

    template<typename L, typename tGradCollector>
    std::true_type GradCollectTest(decltype(&L::template GradCollect<tGradCollector>));
    template<typename L, typename tGradCollector>
    std::false_type GradCollectTest(...);
    template<typename tLayer, typename tGradCollector>
    void LayerGradCollect(tLayer &layer, tGradCollector &gc)
    {
        if constexpr(decltype(GradCollectTest<tLayer, tGradCollector>(nullptr))::value)
        {
            layer.GradCollect(gc);
        }
    }

    template<typename L, typename tSaveWeights>
    std::true_type SaveWeightsTest(decltype(&L::template SaveWeights<tSaveWeights>));    
    template<typename L, typename tSaveWeights>
    std::false_type SaveWeightsTest(...);
    template<typename tLayer, typename tSaveWeights>
    void LayerSaveWeights(tLayer &layer, tSaveWeights &saver)
    {
        if constexpr(decltype(SaveWeightsTest<tLayer, tSaveWeights>(nullptr))::value)
        {
            layer.SaveWeights(saver);
        }
    }

    template<typename L>
    std::true_type NeutralInvariantTest(decltype(&L::NeutralInvariant));
    template<typename L>
    std::false_type NeutralInvariantTest(...);
    template<typename tLayer>
    void LayerNeutralInvariant(tLayer &layer)
    {
        if constexpr(decltype(NeutralInvariantTest<tLayer>(nullptr))::value)
        {
            layer.NeutralInvariant();
        }
    }
}