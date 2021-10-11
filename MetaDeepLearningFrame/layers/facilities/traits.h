#pragma once
#include "../../data_type/dynamic.h"
#include "../../facilities/null_param.h"
#include <stack>
#include <type_traits>

namespace MDL
{
    namespace LayerTraits
    {

        template<typename ElementType, typename DeviceType, typename CateType>
        struct LayerInternalBufType_
        {
            using tmp = DynamicData<ElementType, DeviceType, CateType>;
            using type = std::stack<tmp, std::list<tmp>>;
        };

        template<bool trigger, bool batchMode, typename ElementType, typename DeviceType, typename CateTypeSingle, typename CateTypeBatch>
        struct LayerInternalBuf_
        {
            using type = std::conditional_t<batchMode, LayerInternalBufType_<ElementType, DeviceType, CateTypeBatch>, LayerInternalBufType_<ElementType, DeviceType, CateTypeSingle>>::type;
        };
        template <bool batchMode, typename ElementType, typename DeviceType, typename CateTypeSingle, typename CateTypeBatch>
        struct LayerInternalBuf_<false, batchMode, ElementType, DeviceType, CateTypeSingle, CateTypeBatch>
        {
            using type = NullParameter;
        };

        template<bool trigger, bool batchMode, typename ElementType, typename DeviceType, typename CateTypeSingle, typename CateTypeBatch>
        using LayerInternalBuf = typename LayerInternalBuf_<trigger, batchMode, ElementType, DeviceType, CateTypeSingle, CateTypeBatch>::type;

        template <typename tWeight, typename tGrad, typename tGradCollector>
        void MatrixGradCollect(tWeight &weight, tGrad &grad, tGradCollector &coll)
        {
            while(!grad.empty())
            {
                auto g = grad.top();
                grad.pop();
                //TODO:Collect()
                coll.Collect(weight, g);
            }
        }
    }
}