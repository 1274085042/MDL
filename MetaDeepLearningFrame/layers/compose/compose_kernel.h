#pragma once

#include "../../facilities/traits.h"
#include "../../policies/change_policy.h"
#include "../../facilities/var_type_dict.h"
#include "../../policies/policy_selector.h"
#include "../../policies/policy_container.h"
#include "../../policies/policy_operations.h"
#include "../../layers/facilities/policies.h"
#include "../../layers/facilities/interface_func.h"
#include <tuple>
#include <memory>
#include <type_traits>

/*
* ComposeTopology包含的主要步骤：
* 1 对描述结构语句进行划分，放入相应的容器中
* 2 针对上一步的结果，进行结构合法性检查，对于非法的情况，给出断言，产生编译期其错误
* 3 在确保了输入信息是合法的前提下，进行拓扑排序
* 4 基于拓扑排序的结果，提供元函数Instantiation，在给定Policy的基础上具体化每个子层
*/

/*
* ComposeKernel包含的主要步骤：
* 1 子层对象管理
* 2 参数获取、梯度收集、中性检测
* 3 参数初始化与加载
* 4 正向传播、反向传播
*/

namespace MDL
{
    //--------------------------------------- ComposeTopology --------------------------------------- 
    /*
    * 1 对描述结构语句进行划分，放入相应的容器中
    */
   // 1.1 4种描述结构语句，每个语句就是一个类模板
   // Sublayer描述复合层中包含的子层
   template<typename tLayerTag, template<typename> class tLayer>
   struct Sublayer
   {
       using LayerTag = tLayerTag;
       template<typename T>
       using Layer = tLayer<T>;
   };
    // InConnect描述复合层输入容器中的元素和子层输入容器中的元素之间的联系
    template<typename tInName, typename tInLayerTag, typename tInLayerName>
    struct InConnect
    {
        using InName = tInName;
        using InLayerTag = tInLayerTag;
        using InLayerName = tInLayerName;
    };
    // OutConnect描述子层输出容器中的元素和复合层输出容器中的元素之间的联系
    template<typename tOutLayerTag, typename tOutLayerName, typename tOutName>
    struct OutConnect
    {
        using OutName = tOutName;
        using OutLayerTag = tOutLayerTag;
        using OutLayerName = tOutLayerName;
    };
    // InternalConnect描述子层之间的连接关系
    template<typename tOutLayerTag, typename tOutLayerName, typename tInLayerTag, typename tInLayerName>
    struct InternalConnect
    {
        using OutLayerTag = tOutLayerTag;
        using OutLayerName = tOutLayerName;
        using InLayerTag = tInLayerTag;
        using InLayerName = tInLayerName;
    };

    // 1.2 4个不同的容器存放四种不同的描述句
    template<typename ...T>
    struct SublayerContainer;
    template<typename ...T>
    struct InConnectContainer;
    template<typename ...T>
    struct OutConnectContainer;
    template<typename ...T>
    struct InternalConnectContainer;
    
    // 1.3 划分描述结构语句，并放入相应的容器中
    template<typename ...tParameters>
    struct SeparateParameters_
    {
        template<typename tSublayerContainer, typename tInConnectContainer, typename tOutConnectContainer, typename tInternalConnectContainer, typename ...T>
        struct SeparateParametersHelper
        {
            static_assert(!sizeof...(T), "Error separate parameters");
            using SublayerContainerRes = tSublayerContainer;
            using InConnectContainerRes = tInConnectContainer;
            using OutConnectContainerRes = tOutConnectContainer;
            using InternalConnectContainerRes = tInternalConnectContainer;
        };
        template <typename ...tSublayers, typename ...tInConnects, typename ...tOutConnects, typename ...tInternalConnects, typename tLayerTag, template <typename> class tLayer, typename ...T>
        struct SeparateParametersHelper<SublayerContainer<tSublayers...>, InConnectContainer<tInConnects...>, OutConnectContainer<tOutConnects...>, InternalConnectContainer<tInternalConnects...>, Sublayer<tLayerTag, tLayer>, T...>
        {
            using Res = SeparateParametersHelper<SublayerContainer<tSublayers..., Sublayer<tLayerTag, tLayer>>, InConnectContainer<tInConnects...>, OutConnectContainer<tOutConnects...>, InternalConnectContainer<tInternalConnects...>, T...>;
            
            using SublayerContainerRes = typename Res::SublayerContainerRes;
            using InConnectContainerRes = typename Res::InConnectContainerRes;
            using OutConnectContainerRes = typename Res::OutConnectContainerRes;
            using InternalConnectContainerRes = typename Res::InternalConnectContainerRes;
        };
        template <typename ...tSublayers, typename ...tInConnects, typename ...tOutConnects, typename ...tInternalConnects, typename tInName, typename tInLayerTag, typename tInLayerName, typename ...T>
        struct SeparateParametersHelper<SublayerContainer<tSublayers...>, InConnectContainer<tInConnects...>, OutConnectContainer<tOutConnects...>, InternalConnectContainer<tInternalConnects...>, InConnect<tInName, tInLayerTag, tInLayerName>, T...>
        {
            using Res =SeparateParametersHelper<SublayerContainer<tSublayers...>, InConnectContainer<tInConnects..., InConnect<tInName, tInLayerTag, tInLayerName>>, OutConnectContainer<tOutConnects...>, InternalConnectContainer<tInternalConnects...>, T...>;
            
            using SublayerContainerRes = typename Res::SublayerContainerRes;
            using InConnectContainerRes = typename Res::InConnectContainerRes;
            using OutConnectContainerRes = typename Res::OutConnectContainerRes;
            using InternalConnectContainerRes = typename Res::InternalConnectContainerRes;
        };
        template <typename ...tSublayers, typename ...tInConnects, typename ...tOutConnects, typename ...tInternalConnects, typename tOutLayerTag, typename tOutLayerName, typename tOutName, typename ...T>
        struct SeparateParametersHelper<SublayerContainer<tSublayers...>, InConnectContainer<tInConnects...>, OutConnectContainer<tOutConnects...>, InternalConnectContainer<tInternalConnects...>, OutConnect<tOutLayerTag, tOutLayerName, tOutName>, T...>
        {
            using Res = SeparateParametersHelper<SublayerContainer<tSublayers...>, InConnectContainer<tInConnects...>, OutConnectContainer<tOutConnects..., OutConnect<tOutLayerTag, tOutLayerName, tOutName>>, InternalConnectContainer<tInternalConnects...>, T...>;
            
            using SublayerContainerRes = typename Res::SublayerContainerRes;
            using InConnectContainerRes = typename Res::InConnectContainerRes;
            using OutConnectContainerRes = typename Res::OutConnectContainerRes;
            using InternalConnectContainerRes = typename Res::InternalConnectContainerRes;
        };
        template <typename ...tSublayers, typename ...tInConnects, typename ...tOutConnects, typename ...tInternalConnects, typename tOutLayerTag, typename tOutLayerName, typename tInLayerTag, typename tInLayerName, typename ...T>
        struct SeparateParametersHelper<SublayerContainer<tSublayers...>, InConnectContainer<tInConnects...>, OutConnectContainer<tOutConnects...>, InternalConnectContainer<tInternalConnects...>, InternalConnect<tOutLayerTag, tOutLayerName, tInLayerTag, tInLayerName>, T...>
        {
            using Res = SeparateParametersHelper<SublayerContainer<tSublayers...>, InConnectContainer<tInConnects...>, OutConnectContainer<tOutConnects...>, InternalConnectContainer<tInternalConnects..., InternalConnect<tOutLayerTag, tOutLayerName, tInLayerTag, tInLayerName>>,  T...>;
            
            using SublayerContainerRes = typename Res::SublayerContainerRes;
            using InConnectContainerRes = typename Res::InConnectContainerRes;
            using OutConnectContainerRes = typename Res::OutConnectContainerRes;
            using InternalConnectContainerRes = typename Res::InternalConnectContainerRes;
        };

        using tmp = SeparateParametersHelper<SublayerContainer<>, InConnectContainer<>, OutConnectContainer<>, InternalConnectContainer<>, tParameters...>;
        using SublayerContainerRes = typename tmp::SublayerContainerRes;
        using InConnectContainerRes = typename tmp::InConnectContainerRes;
        using OutConnectContainerRes = typename tmp::OutConnectContainerRes;
        using InternalConnectContainerRes = typename tmp::InternalConnectContainerRes;
    };

     /*
    * 2 结构合法性检查
    */
    template <typename tCurTag, typename ...tArray>
    struct TagExistInLayerComps_
    {
        template <typename ...T>
        struct Impl_
        {
            static constexpr bool value = false;
        };

        template <typename tLayerTag, template<typename> class tLayer, typename ...T>
        struct Impl_<Sublayer<tLayerTag, tLayer>, T...>
        {
            static constexpr bool tmp = std::is_same<tCurTag, tLayerTag>::value;
            static constexpr bool value = OrValue<tmp, Impl_<T...>>;
        };

        template <typename tInName, typename tInLayerTag, typename tInLayerName, typename ...T>
        struct Impl_<InConnect<tInName, tInLayerTag, tInLayerName>, T...>
        {
            static constexpr bool tmp = std::is_same<tCurTag, tInLayerTag>::value;
            static constexpr bool value = OrValue<tmp, Impl_<T...>>;
        };

        template <typename tOutLayerTag, typename tOutLayerName, typename tOutName, typename ...T>
        struct Impl_<OutConnect<tOutLayerTag, tOutLayerName, tOutName>, T...>
        {
            static constexpr bool tmp = std::is_same<tCurTag, tOutLayerTag>::value;
            static constexpr bool value = OrValue<tmp, Impl_<T...>>;
        };

        template <typename tOutLayerTag, typename tOutLayerName, typename tInLayerTag, typename tInLayerName, typename ...T>
        struct Impl_<InternalConnect<tOutLayerTag, tOutLayerName, tInLayerTag, tInLayerName>, T...>
        {
            static constexpr bool tmp1 = std::is_same<tCurTag, tOutLayerTag>::value;
            static constexpr bool tmp2 = std::is_same<tCurTag, tInLayerTag>::value;
            static constexpr bool tmp3 = tmp1 || tmp2;
            static constexpr bool value = OrValue<tmp3, Impl_<T...>>;
        };

        static constexpr bool value = Impl_<tArray...>::value;
    };

    // 2.1 位于同一个复合层中的每个子层必须具有不同的名称，不同的子层可以具有不同的类型，但名称不能相同，
    // 因为名称描述了子层之间的连接关系，如果名称相同，连接关系将无法确定
    template <typename tSublayerContainerRes>
    struct SublayerTagCheck;
    template <typename ...tSublayers>
    struct SublayerTagCheck<SublayerContainer<tSublayers...>>
    {
        template<typename ...tSub>
        struct UniqueLayerTagCheck_
        {
            static constexpr bool value = true;
        };
        template<typename tCurSublayer, typename ...tSub>
        struct UniqueLayerTagCheck_<tCurSublayer, tSub...>
        {  
            using tag = typename tCurSublayer::LayerTag;
            static constexpr bool tmp = !(TagExistInLayerComps_<tag, tSub...>::value);
            static constexpr bool value = AndValue<tmp, UniqueLayerTagCheck_<tSub...>>;
        };

        static constexpr bool isUniqueTag = UniqueLayerTagCheck_<tSublayers...>::value;
    };

    // 2.2 在一条InternalConnect语句中，输入层与输出层不能相同，否则将构成环，这是检测环最简单的情况
    // 2.3 输入层的输入容器数据来源必须是唯一的，InternalConnect语句中，输入层的输入来源必须是唯一的
    template<typename tInternalConnectContainerRes>
    struct InternalConnectCheck;
    template<typename ...tInternalConnects>
    struct InternalConnectCheck<InternalConnectContainer<tInternalConnects...>>
    {
        template <typename tInTag, typename tInName, typename ...T>
        struct InternalSourceUniqueCheckHelper_
        {
            static constexpr bool value = true;
        };
        template <typename tInTag, typename tInName, typename tCheck, typename ...T>
        struct InternalSourceUniqueCheckHelper_<tInTag, tInName, tCheck, T...>
        {
            using CheckTag = typename tCheck::InLayerTag;
            using CheckName = typename tCheck::InLayerName;
            static constexpr bool tmp1 = std::is_same<tInTag, CheckTag>::value;
            static constexpr bool tmp2 = std::is_same<tInName, CheckName>::value;
            static constexpr bool tmp3 = !(tmp1 && tmp2);
            static constexpr bool value = AndValue<tmp3, InternalSourceUniqueCheckHelper_<tInTag, tInName, T...>>;
        };

        template<typename ...tIC>
        struct InternalSourceUniqueCheck_
        {
            static constexpr bool value = true;
        };
        template<typename tCur, typename ...tIC>
        struct InternalSourceUniqueCheck_<tCur, tIC...>
        {
            using CurTag = typename tCur::InLayerTag;
            using CurName = typename tCur::InLayerName;
            static constexpr bool tmp = InternalSourceUniqueCheckHelper_<CurTag, CurName, tIC...>::value;
            static constexpr bool value = AndValue<tmp, InternalSourceUniqueCheck_<tIC...>>;
        };

        template<typename ...tIC>
        struct SelfCycleCheck_
        {
            static constexpr bool value = true;
        };
        template<typename tCur, typename ...tIC>
        struct SelfCycleCheck_<tCur, tIC...>
        {
            using OutTag = typename tCur::OutLayerTag;
            using InTag = typename tCur::InLayerTag;
            static constexpr bool tmp = !(std::is_same<OutTag, InTag>::value);
            static constexpr bool value = AndValue<tmp, SelfCycleCheck_<tIC...>>;
        };

        static constexpr bool noSelfCycle = SelfCycleCheck_<tInternalConnects...>::value;
        static constexpr bool sourceUnique = InternalSourceUniqueCheck_<tInternalConnects...>::value;
    };

    // 2.3 输入层的输入容器数据来源必须是唯一的，InConnect语句中，输入层的输入来源必须是唯一的
    template<typename tInConnectContainerRes>
    struct InputConnectCheck;

    template<typename ...tInConnects>
    struct InputConnectCheck<InConnectContainer<tInConnects...>>
    {
        template<typename tCurTag, typename tCurName, typename ...T>
        struct InSourceUniqueCheckHelper_
        {
            static constexpr bool value = true;
        };
        template<typename tCurTag, typename tCurName, typename tCheck, typename ...T>
        struct InSourceUniqueCheckHelper_<tCurTag, tCurName, tCheck, T...>
        {
            using CheckTag = typename tCheck::InLayerTag;
            using CheckName = typename tCheck::InLayerName;
            static constexpr bool tmp1 = std::is_same<tCurTag,CheckTag>::value;
            static constexpr bool tmp2 = std::is_same<tCurName,CheckName>::value;
            static constexpr bool tmp3 = !(tmp1&&tmp2);
            static constexpr bool value = AndValue<tmp3, InSourceUniqueCheckHelper_<tCurTag, tCurName, T...>>;
        };

        template<typename ...tIC>
        struct InSourceUniqueCheck_
        {
            static constexpr bool value = true;
        };

        template<typename tCur, typename ...tIC>
        struct InSourceUniqueCheck_<tCur, tIC...>
        {
            using CurTag = typename tCur::InLayerTag;
            using CurName = typename tCur::InLayerName;

            static constexpr bool tmp = InSourceUniqueCheckHelper_<CurTag, CurName, tIC...>::value;
            static constexpr bool value = AndValue<tmp, InSourceUniqueCheck_<tIC...>>;
        };

        static constexpr bool sourceUnique = InSourceUniqueCheck_<tInConnects...>::value;
    };

    // 2.3 输入容器数据来源必须是唯一的，OutConnect语句中，输入容器的输入来源必须是唯一的
    template<typename tOutConnectContainerRes>
    struct OutputConnectCheck;

    template<typename ...tOutConnects>
    struct OutputConnectCheck<OutConnectContainer<tOutConnects...>>
    {
        template<typename tCurName, typename ...T>
        struct OutSourceUniqueCheckHelper_
        {
            static constexpr bool value = true;
        };
        template<typename tCurName, typename tCheck, typename ...T>
        struct OutSourceUniqueCheckHelper_<tCurName, tCheck, T...>
        {
            using CheckName = typename tCheck::OutName;
            static constexpr bool tmp = std::is_same<tCurName,CheckName>::value;
            static constexpr bool value = AndValue<!tmp, OutSourceUniqueCheckHelper_<tCurName, T...>>;
        };

        template<typename ...tIC>
        struct OutSourceUniqueCheck_
        {
            static constexpr bool value = true;
        };

        template<typename tCur, typename ...tIC>
        struct OutSourceUniqueCheck_<tCur, tIC...>
        {
            using CurName = typename tCur::OutName;

            static constexpr bool tmp = OutSourceUniqueCheckHelper_<CurName, tIC...>::value;
            static constexpr bool value = AndValue<tmp, OutSourceUniqueCheck_<tIC...>>;
        };

        static constexpr bool sourceUnique = OutSourceUniqueCheck_<tOutConnects...>::value;
    };

    // 2.4 InConnect、OutConnect和InternalConnect中层的名称必须和某个Sublayer对应
    template<typename tInternalConnectContainerRes, typename tSublayerContainerRes>
    struct InternalTagInSublayer;
    template<typename ...tInternalConnects, typename ...tSublayers>
    struct InternalTagInSublayer<InternalConnectContainer<tInternalConnects...>, SublayerContainer<tSublayers...>>
    {
        template<typename ...T>
        struct Impl_
        {
            static constexpr bool value = true;
        };
        template<typename tCur, typename ...T>
        struct Impl_<tCur, T...>
        {
            using CurOutLayerTag = typename tCur::OutLayerTag;
            using CurInLayerTag =  typename tCur::InLayerTag;

            static constexpr bool tmp1 = TagExistInLayerComps_<CurOutLayerTag, tSublayers...>::value;
            static constexpr bool tmp2 = TagExistInLayerComps_<CurInLayerTag, tSublayers...>::value;
            static constexpr bool tmp3 =  tmp1 && tmp2;
            static constexpr bool value = AndValue<tmp3, Impl_<T...>>;
        };

        static constexpr bool value = Impl_<tInternalConnects...>::value;
    };

    // 2.4 InConnect、OutConnect和InternalConnect中层的名称必须和某个Sublayer对应
    template<typename tInConnectContainerRes, typename tSublayerContainerRes>
    struct InputTagInSublayer;
    template<typename ...tInConnects, typename ...tSublayers>
    struct InputTagInSublayer<InConnectContainer<tInConnects...>, SublayerContainer<tSublayers...>>
    {
        template<typename ...T>
        struct Impl_
        {
            static constexpr bool value = true;
        };
        template<typename tCur, typename ...T>
        struct Impl_<tCur, T...>
        {
            using CurInLayerTag = typename tCur::InLayerTag;;

            static constexpr bool tmp = TagExistInLayerComps_<CurInLayerTag, tSublayers...>::value;
            static constexpr bool value = AndValue<tmp, Impl_<T...>>;
        };

        static constexpr bool value = Impl_<tInConnects...>::value;
    };

    // 2.4 InConnect、OutConnect和InternalConnect中层的名称必须和某个Sublayer对应
    template<typename tOutConnectContainerRes, typename tSublayerContainerRes>
    struct OutputTagInSublayer;
    template<typename ...tOutConnects, typename ...tSublayers>
    struct OutputTagInSublayer<OutConnectContainer<tOutConnects...>, SublayerContainer<tSublayers...>>
    {
        template<typename ...T>
        struct Impl_
        {
            static constexpr bool value = true;
        };
        template<typename tCur, typename ...T>
        struct Impl_<tCur, T...>
        {
            using CurOutLayerTag = typename tCur::OutLayerTag;;

            static constexpr bool tmp = TagExistInLayerComps_<CurOutLayerTag, tSublayers...>::value;
            static constexpr bool value = AndValue<tmp, Impl_<T...>>;
        };

        static constexpr bool value = Impl_<tOutConnects...>::value;
    };

    // 2.5 Sublayer中的层必须出现在InConnect、OutConnect或者InternalConnect中，否则，这个层便会称为僵尸层
    template<typename tSublayerContainerRes, typename tInConnectContainerRes, typename tOutConnectContainerRes, typename tInternalConnectContainerRes>
    struct SublayerTagInOtherArray;
    template<typename ...tSublayers, typename ...tInConnects, typename ...tOutConnects, typename ...tInternalConnects>
    struct SublayerTagInOtherArray<SublayerContainer<tSublayers...>, InConnectContainer<tInConnects...>, OutConnectContainer<tOutConnects...>, InternalConnectContainer<tInternalConnects...>>
    {
        template<typename ...T>
        struct Impl_
        {
            static constexpr bool value = true;
        };
        template<typename tCur, typename ...T>
        struct Impl_<tCur, T...>
        {
            using CurLayerTag = typename tCur::LayerTag;
            static constexpr bool tmp1 = TagExistInLayerComps_<CurLayerTag, tInConnects...>::value;
            static constexpr bool tmp2 = TagExistInLayerComps_<CurLayerTag, tOutConnects...>::value;
            static constexpr bool tmp3 = TagExistInLayerComps_<CurLayerTag, tInternalConnects...>::value;
            static constexpr bool tmp4 = tmp1 || tmp2 || tmp3;
            static constexpr bool value = AndValue<tmp4, Impl_<T...>>;
        };

        static constexpr bool value = Impl_<tSublayers...>::value;
    };

    // 2.6 InternalConnect的输出要么作为某个子层的输入，要么作为复合层的输出，不能悬空，否则反向传播时将无法处理
    template<typename tCurTag, typename ...T>
    struct InTagInternalOutTag_
    {
        static constexpr bool value = false;
    };
    template<typename tCurTag, typename tOutLayerTag, typename tOutLayerName, typename tInLayerTag, typename tInLayerName, typename ...T>
    struct InTagInternalOutTag_<tCurTag, InternalConnect<tOutLayerTag, tOutLayerName, tInLayerTag, tInLayerName>, T...>
    {
        static constexpr bool tmp = std::is_same<tCurTag, tOutLayerTag>::value;
        static constexpr bool value = OrValue<tmp, InTagInternalOutTag_<tCurTag, T...>>;
    };

    template<typename tInternalConnectContainerRes, typename tOutConnectContainerRes>
    struct InternalInTagUseful;
    template<typename ...tInternalConnects, typename ...tOutConnects>
    struct InternalInTagUseful<InternalConnectContainer<tInternalConnects...>, OutConnectContainer<tOutConnects...>>
    {
        template<typename ...T>
        struct Impl_
        {
            static constexpr bool value = true;
        };
        template<typename tCur, typename ...T>
        struct Impl_<tCur, T...>
        {
            using CurInLayerTag = typename tCur::InLayerTag;

            static constexpr bool tmp1 = InTagInternalOutTag_<CurInLayerTag, tInternalConnects...>::value;
            static constexpr bool tmp2 = TagExistInLayerComps_<CurInLayerTag, tOutConnects...>::value;
            static constexpr bool tmp3 = tmp1 || tmp2;
            static constexpr bool value = AndValue<tmp3, Impl_<T...>>;
        };

        static constexpr bool value = Impl_<tInternalConnects...>::value;
    };
    
    // 2.7 InConnect出现的层要么连接到InternalConnect，要么连接到OutConnect
    template<typename tInConnectContainerRes, typename tInternalConnectContainerRes, typename tOutConnectContainerRes>
    struct InputLayerUserful;
    template<typename ...tInConnects, typename ...tInternalConnects, typename ...tOutConnects>
    struct InputLayerUserful<InConnectContainer<tInConnects...>, InternalConnectContainer<tInternalConnects...>, OutConnectContainer<tOutConnects...>>
    {
        template<typename ...T>
        struct Impl_
        {
            static constexpr bool value = true;
        };
        template<typename tCur, typename ...T>
        struct Impl_<tCur, T...>
        {
            using CurLayerTag = typename tCur::InLayerTag;
            static constexpr bool tmp1 = InTagInternalOutTag_<CurLayerTag, tInternalConnects...>::value;
            static constexpr bool tmp2 = TagExistInLayerComps_<CurLayerTag, tOutConnects...>::value;
            static constexpr bool tmp3 = tmp1 || tmp2;
            static constexpr bool value = AndValue<tmp3, Impl_<T...>>;
        };

        static constexpr bool value = Impl_<tInConnects...>::value;  
    };

    /*
    * 3 拓扑排序
    */
    template <typename tCurTag, typename ...T>
    struct TagExist_
    {
        static constexpr bool value = false;
    };
    template <typename tCurTag, typename tCheckTag, typename ...T>
    struct TagExist_<tCurTag, tCheckTag, T...>
    {
        static constexpr bool tmp = std::is_same<tCurTag, tCheckTag>::value;
        static constexpr bool value = OrValue<tmp, TagExist_<tCurTag, T...>>;
    };

    template<typename tCurTag, typename ...T>
    struct OutTagInternalInTag_
    {
        static constexpr bool value = false;
    };
    template<typename tCurTag, typename tOutLayerTag, typename tOutLayerName, typename tInLayerTag, typename tInLayerName, typename ...T>
    struct OutTagInternalInTag_<tCurTag, InternalConnect<tOutLayerTag, tOutLayerName, tInLayerTag, tInLayerName>, T...>
    {
        static constexpr bool tmp = std::is_same<tCurTag, tInLayerTag>::value;
        static constexpr bool value = OrValue<tmp, OutTagInternalInTag_<tCurTag, T...>>;
    };

    template<typename ...T>
    struct TagContainer;

    template<typename tOrderRes, typename tUnOrderRes, typename tCheckInternals>
    struct MainLoop_
    {
        using Order = tOrderRes;
        using UnOrder = tUnOrderRes;
    };
    template<typename ...tOrders, typename ...tUnOrders, typename tCur, typename ...T>
    struct MainLoop_<SublayerContainer<tOrders...>, SublayerContainer<tUnOrders...>, InternalConnectContainer<tCur, T...>>
    {
        template <typename tRemainInternalConnectContainer, typename tInternalConnectContainer, typename tTagContainer, typename ...T3>
        struct InternalLayerPrune_
        {
            using type = tRemainInternalConnectContainer;
            using NewTagContainer = tTagContainer;
        };

        template <typename ...T1, typename ...T2, typename ...tTags, typename tCheck, typename ...T3>
        struct InternalLayerPrune_<InternalConnectContainer<T1...>, InternalConnectContainer<T2...>, TagContainer<tTags...>, tCheck, T3...>
        {
            using CheckTag = typename tCheck::OutLayerTag;
            static constexpr bool b1 = OutTagInternalInTag_<CheckTag, T2...>::value;

            template<bool b, typename Dummpy = void>
            struct Help_
            {
                using RemainInternalConnectContainer = InternalConnectContainer<T1..., tCheck>;
                using HelpTagContainer = TagContainer<tTags...>;
            };
            template<typename Dummpy>
            struct Help_<false, Dummpy>
            {
                static constexpr bool b2 = TagExist_<CheckTag, tTags...>::value;
                using RemainInternalConnectContainer = InternalConnectContainer<T1...>;

                using HelpTagContainer = std::conditional_t<b2, TagContainer<tTags...>, TagContainer<tTags..., CheckTag>> ;
            };

            using TmpRemainInternalConnectContainer = typename Help_<b1>::RemainInternalConnectContainer;
            using TmpHelpTagContainer = typename Help_<b1>::HelpTagContainer;

            using TmpStep = InternalLayerPrune_<TmpRemainInternalConnectContainer, InternalConnectContainer<T2...>, TmpHelpTagContainer, T3...>;

            using type = typename TmpStep::type;
            using NewTagContainer = typename TmpStep::NewTagContainer;
        };
        
        template<typename tInput, typename tOr, typename tUnOr, typename tTags>
        struct SeparateByPostTag_
        {
            using Order = tOr;
            using UnOrder = tUnOr;
        };
        template<typename tCheck, typename ...T1, typename ...T2, typename ...T3, typename ...T4>
        struct SeparateByPostTag_<SublayerContainer<tCheck, T1...>, SublayerContainer<T2...>, SublayerContainer<T3...>, TagContainer<T4...>>
        {
            using CurTag = typename tCheck::LayerTag;
            static constexpr bool b = TagExist_<CurTag, T4...>::value;
            using tmpOrder = std::conditional_t<b, SublayerContainer<T2..., tCheck>, SublayerContainer<T2...>>;
            using tmpUnOrder = std::conditional_t<b, SublayerContainer<T3...>, SublayerContainer<T3..., tCheck>>;
            using tmpStep = SeparateByPostTag_<SublayerContainer<T1...>, tmpOrder, tmpUnOrder, TagContainer<T4...>>;
            using Order = typename tmpStep::Order;
            using UnOrder = typename tmpStep::UnOrder;
        };

        using AllInternalConnect = InternalConnectContainer<tCur, T...>;
        using Tmp1 = InternalLayerPrune_<InternalConnectContainer<>, AllInternalConnect, TagContainer<>, tCur, T...>;
        using TmpInternalContainer= typename Tmp1::type;
        static_assert(ArraySize<TmpInternalContainer> < ArraySize<AllInternalConnect>,"Cycle in compose!");

        using TmpTagContainer = typename Tmp1::NewTagContainer;
        
        using Tmp2 = SeparateByPostTag_<SublayerContainer<tUnOrders...>, SublayerContainer<tOrders...>, SublayerContainer<>, TmpTagContainer>;
        using TmpOrder = typename Tmp2::Order;
        using TmpUnorder = typename Tmp2::UnOrder;

        using TmpStep = MainLoop_<TmpOrder, TmpUnorder, TmpInternalContainer>;
        using Order = typename TmpStep::Order;
        using UnOrder = typename TmpStep::UnOrder;
    };

    template<typename tSublayerContainer, typename tInternalConnectContainer>
    struct TopologicalOrdering_;
    template<typename ...tSubLayers, typename ...tInternalConnects>
    struct TopologicalOrdering_<SublayerContainer<tSubLayers...>, InternalConnectContainer<tInternalConnects...>>
    {
        // 3.1 预处理，将独立的层放入排序结果中
        template <typename tOrderRes, typename tUnOrderRes, typename ...T>
        struct SublayerPreprocess_
        {
            using Order = tOrderRes;
            using UnOrder = tUnOrderRes;
        };
        template <typename ...tOrders, typename ...tUnOrders, typename tCur, typename ...T>
        struct SublayerPreprocess_<SublayerContainer<tOrders...>, SublayerContainer<tUnOrders...>, tCur, T...>
        {
            using CurTag = typename tCur::LayerTag;
            static constexpr bool b = TagExistInLayerComps_<CurTag, tInternalConnects...>::value;
            using TmpOrder = std::conditional_t<b, SublayerContainer<tOrders...>, SublayerContainer<tOrders..., tCur>>;
            using TmpUnOrder = std::conditional_t<b, SublayerContainer<tUnOrders..., tCur>, SublayerContainer<tUnOrders...>>;

            using TmpStep = SublayerPreprocess_<TmpOrder, TmpUnOrder, T...>;
            using Order = typename TmpStep::Order;
            using UnOrder = typename TmpStep::UnOrder;
        };
        using OrderResAfterPreProcess = typename SublayerPreprocess_<SublayerContainer<>, SublayerContainer<>, tSubLayers...>::Order;
        using UnOrderResAfterPreProcess = typename SublayerPreprocess_<SublayerContainer<>, SublayerContainer<>, tSubLayers...>::UnOrder;

        // 3.2 主逻辑
        using MainLoopFunc = MainLoop_<OrderResAfterPreProcess, UnOrderResAfterPreProcess, InternalConnectContainer<tInternalConnects...>>;
        using OrderResAfterMainLoop = typename MainLoopFunc::Order;
        using UnOrderResAfterMainLoop = typename MainLoopFunc::UnOrder;

        // 3.3 后处理，将剩余的层放入排序结果中
        // template<typename T1, typename T2>
        // struct CascadSublayers_
        // {
        //     using type = T1;
        // };
        // template <typename ...tOrders, typename tCur, typename ...tUnOrders>
        // struct CascadSublayers_<SublayerContainer<tOrders...>, SublayerContainer<tCur, tUnOrders...>>
        // {
        //     using tmp = SublayerContainer<tOrders..., tCur>;
        //     using type = typename CascadSublayers_<tmp, SublayerContainer<tUnOrders...>>::type;
        // };

        template<typename T1, typename T2>
        struct CascadSublayers_;
        template <typename ...T1, typename ...T2>
        struct CascadSublayers_<SublayerContainer<T1...>, SublayerContainer<T2...>>
        {
            using type = SublayerContainer<T1..., T2...>;
        };

        using type = typename CascadSublayers_<OrderResAfterMainLoop, UnOrderResAfterMainLoop>::type;
    };

    /*
    * 4 Instantiation在policy的基础上对拓扑排序的结果进行具体化
    */
    template <typename ...>
    struct SublayerPolicyContainer;

    template<typename tLayerTag, template <typename > class tLayer, typename tPolicies>
    struct SublayerPolicies
    {
        using Tag = tLayerTag;
        template <typename T>
        using Layer = tLayer<T>;
        using Policies = tPolicies;
    };

    template <typename tPolicyContainer, typename tOrderedSublayerContainer>
    struct GetSublayerPolicy_;
    template<typename tPolicyContainer, typename ...tSublayers>
    struct GetSublayerPolicy_<tPolicyContainer, SublayerContainer<tSublayers...>>
    {
        template<typename tRes, typename ...T>
        struct Impl_
        {   
            using type = tRes;
        };
        template<typename ...T1, typename tCur, typename ...T2 >
        struct Impl_<SublayerPolicyContainer<T1...>, tCur, T2...>
        {
            using CurTag = typename tCur::LayerTag;
            template <typename T>
            using CurName = typename tCur::template Layer<T>;
            using CurPolicy = SubPolicyPicker<tPolicyContainer, CurTag>;
            using tmp = SublayerPolicyContainer<T1..., SublayerPolicies<CurTag, CurName, CurPolicy>>;
            using type = typename Impl_<tmp, T2...>::type;
        };

        using type = typename Impl_<SublayerPolicyContainer<>, tSublayers...>::type;
    };

    template<bool b, typename tSublayerPolicyContainer>
    struct FeedbackOutputCheck_
    {
        static constexpr bool value = true;
    };
    template<typename tCur, typename ...T>
    struct FeedbackOutputCheck_<true, SublayerPolicyContainer<tCur, T...>>
    {
        using CurPolicies = typename tCur::Policies;
        static constexpr bool b = PolicySelect<FeedbackPolicy, CurPolicies>::IsFeedbackOutput;
        static constexpr bool value = AndValue<b, FeedbackOutputCheck_<true, SublayerPolicyContainer<T...>>>;
    };

    template <bool b, typename tChangeLayerTag, typename tNotChangeRes, typename tChangeRes>
    struct UpdateByTag_
    {
        using type = tNotChangeRes;
    };
    template <typename tChangeLayerTag, typename ...T, typename tChangeRes>
    struct UpdateByTag_<true, tChangeLayerTag, SublayerPolicyContainer<T...> , tChangeRes>
    {
        using type = tChangeRes;
    };
    template < typename tChangeLayerTag, typename tCur, typename ...T, typename ...tChanges>
    struct UpdateByTag_<true, tChangeLayerTag, SublayerPolicyContainer<tCur, T...>,  SublayerPolicyContainer<tChanges...>> //FeedbackOutSet_ 第三层循环，遍历除当前层之外的其余SublayerPolicies
    {
        using OriTag = typename tCur::Tag;
        template<typename T1>
        using OriLayer = typename tCur::template Layer<T1>;
        using OriPolicies = typename tCur::Policies;

        static constexpr bool b = std::is_same<tChangeLayerTag, OriTag>::value;
        using NewPolicies = typename std::conditional_t<b, ChangePolicy_<PFeedbackOutput, OriPolicies>, Identity_<OriPolicies>>::type;

        using tmp = SublayerPolicyContainer<tChanges..., SublayerPolicies<OriTag, OriLayer, NewPolicies>>;

        using type = typename UpdateByTag_<true, tChangeLayerTag, SublayerPolicyContainer<T...>, tmp>::type;
    };

    template<typename tTag, typename tInternalConnectContainer, typename tSublayerPolicyContainer>
    struct UpdateBySourceLayer_
    {
        template<typename tRes, typename tInternalContainer>
        struct Impl_
        {
            using type = tRes;
        };
        template<typename tCur, typename ...T, typename ...tNotChanges>
        struct Impl_<SublayerPolicyContainer<tNotChanges...>, InternalConnectContainer<tCur, T...>>  //FeedbackOutSet_ 第二层循环，遍历InternalConnectContainer容器，找到需要输出梯度的后继层
        {
            using CheckLayerTag = typename tCur::OutLayerTag;
            using ChageLyaerTag = typename tCur::InLayerTag;
            static constexpr bool b = std::is_same<tTag, CheckLayerTag>::value;

            using tmp = typename UpdateByTag_<b, ChageLyaerTag, SublayerPolicyContainer<tNotChanges...>, SublayerPolicyContainer<>>::type;

            using type = typename Impl_<tmp, InternalConnectContainer<T...>>::type;
        };

        using type = typename Impl_<tSublayerPolicyContainer, tInternalConnectContainer>::type;
    };

    template<typename tSubLayerWithPolicyContainer, typename tInternalConnectContainer>
    struct FeedbackOutSet_;
    template<typename ...T1, typename tInternalConnectContainer>
    struct FeedbackOutSet_<SublayerPolicyContainer<T1...>, tInternalConnectContainer>
    {
        template<typename tRes, typename tNotProcessed>
        struct Impl_
        {
            using type = tRes;
        };
        template<typename ...tProcessed, typename tCur, typename ...T2>
        struct Impl_<SublayerPolicyContainer<tProcessed...>, SublayerPolicyContainer<tCur, T2...>>      // FeedbackOutSet_ 第一层循环，遍历tSubLayerWithPolicyContainer
        {
            using CurTag = typename tCur::Tag;
            using CurPolicies = typename tCur::Policies;

            static constexpr bool b = PolicySelect<FeedbackPolicy, CurPolicies>::IsFeedbackOutput || PolicySelect<FeedbackPolicy, CurPolicies>::IsUpdate;
            using TmpSublayerPolicyContainer = SublayerPolicyContainer<tProcessed..., tCur>;

            using TmpChangeSublayerPolicyContainer = typename std::conditional_t<b, UpdateBySourceLayer_<CurTag, tInternalConnectContainer, SublayerPolicyContainer<T2...>>, Identity_<SublayerPolicyContainer<T2...>>>::type;

            using type = typename Impl_<TmpSublayerPolicyContainer, TmpChangeSublayerPolicyContainer>::type;
        };

        using type = typename Impl_<SublayerPolicyContainer<>, SublayerPolicyContainer<T1...>>::type; 
    };

    template<typename tLayerTag, typename tLayer>
    struct SpecialSublayer
    {
        using Tag = tLayerTag;
        using Layer = tLayer;
    };

    template<typename tRes, typename tSublayerPolicyContainer>
    struct Specialization_
    {
        using type = tRes;
    };
    template<typename ...T1, typename tCur, typename ...T2>
    struct Specialization_<std::tuple<T1...>, SublayerPolicyContainer<tCur, T2...>>
    {   
        using Tag = typename tCur::Tag;
        template <typename T>
        using Layer = typename tCur::template Layer<T>;
        using Policies = typename tCur::Policies;
        using SpecLayer = Layer<Policies>;
        using Tmp = std::tuple<T1..., SpecialSublayer<Tag, SpecLayer>>;
        using type = typename Specialization_<Tmp, SublayerPolicyContainer<T2...>>::type;
    };

    template <typename tPolicyContainer, typename tOrderedSublayerContainer, typename tInternalConnectContainer>
    struct SublayerSpecialization_
    {
        static_assert(IsPolicyContainer<tPolicyContainer>, "Not a policy container");

        // 4.1 根据拓扑排序的结果，获取每个子层的policy
        using SublayerWithPolicyContainer = typename GetSublayerPolicy_<tPolicyContainer, tOrderedSublayerContainer>::type;

        // 4.2 输出梯度行为检测，如果复合层要求输出梯度，那么每个子层的都应该设置IsFeedbackOutput为true
        using PlainPolicies = PlainPolicy<tPolicyContainer>;
        constexpr static bool IsFeedbackOut = PolicySelect<FeedbackPolicy, PlainPolicies>::IsFeedbackOutput;
        static_assert(FeedbackOutputCheck_<IsFeedbackOut, SublayerWithPolicyContainer>::value, "Sublayer policy not set feedback output, logic error!");

        // 4.3 policy修正，如果前驱层需要输出梯度或者参数更新，那么后继层就需要输出梯度
        using FeedbackOutUpdate = typename std::conditional_t<IsFeedbackOut, Identity_<SublayerWithPolicyContainer>, FeedbackOutSet_<SublayerWithPolicyContainer, tInternalConnectContainer>>::type;

        // 4.4 对每一层进行具体化
        using type = typename Specialization_<std::tuple<>, FeedbackOutUpdate>::type;
    };

    template <typename... T>
    struct ComposeTopology
    {
        // Separate Results
        using SubLayers = typename SeparateParameters_<T...>::SublayerContainerRes;
        using InterConnects = typename SeparateParameters_<T...>::InternalConnectContainerRes;
        using InputConnects = typename SeparateParameters_<T...>::InConnectContainerRes;
        using OutputConnects = typename SeparateParameters_<T...>::OutConnectContainerRes;

        // Asserts
        static_assert((ArraySize<SubLayers> != 0), "Sublayer is empty.");
        static_assert((SublayerTagCheck<SubLayers>::isUniqueTag), "Two or more sublayers have same tag.");
        static_assert((InternalConnectCheck<InterConnects>::noSelfCycle), "Internal connections have self-connect.");
        static_assert((InternalConnectCheck<InterConnects>::sourceUnique), "One internal input corresponds to two or more internal outputs.");
        static_assert((InputConnectCheck<InputConnects>::sourceUnique), "One input corresponds to two or more sources.");
        static_assert((OutputConnectCheck<OutputConnects>::sourceUnique), "One output corresponds to two or more sources.");
        static_assert((InternalTagInSublayer<InterConnects, SubLayers>::value), "Internal connections have tags are not sublayer tags.");
        static_assert((InputTagInSublayer<InputConnects, SubLayers>::value), "One or more input tags are not sublayer tags.");
        static_assert((OutputTagInSublayer<OutputConnects, SubLayers>::value), "One or more output tags are not sublayer tags.");
        static_assert((SublayerTagInOtherArray<SubLayers, InputConnects, OutputConnects, InterConnects >::value), "One ore more sublayer tags not belong to any connection containers.");
        static_assert((InternalInTagUseful<InterConnects, OutputConnects>::value), "Internal output info is useless");
        static_assert((InputLayerUserful<InputConnects, InterConnects, OutputConnects>::value), "Input info is useless");

        // Topological Ordering
        // TopologicalOrdering 的形式为 SublayerContainer<Sublayer<layername1, layer1>, Sublayer<layername2, layer2>, Sublayer<layername3, layer3>>
        using TopologicalOrdering = typename TopologicalOrdering_<SubLayers, InterConnects>::type;

        // Specialization 的形式为tuple<SpecialSublayer<layername1, layer1<policies>>, SpecialSublayer<layername2, layer2<policies>>, SpecialSublayer<layername3, layer3<policies>>>
        template <typename tPolicyCont>
        using Specialization = typename SublayerSpecialization_<tPolicyCont, TopologicalOrdering, InterConnects>::type;
    };

  //--------------------------------------- ComposeKernel --------------------------------------- 
    template<typename tSublayerTuple>
    class SublayerArrayMaker
    {
    private:
        template<typename tRes, typename tRem>
        struct SublayerPtrArray_
        {
            using type = tRes;
        };
        template<typename ...tProcessed, typename tCur, typename ...tRemain>
        struct SublayerPtrArray_<std::tuple<tProcessed...>, std::tuple<tCur, tRemain...>>
        {
            using type = typename SublayerPtrArray_<std::tuple<tProcessed..., std::shared_ptr<typename tCur::Layer>>, std::tuple<tRemain...>>::type;
        };

        template<typenaem tTag, size_t n>
        static constexpr size_t Tag2Pos()
        {
            static_assert(!(n>=ArraySize<tSublayerTuple>), "Not find layer" );
            if constexpr(std::is_same<typename std::tuple_element_t<n, tSublayerTuple>::Tag, tTag>::value)
            {   
                return n;
            }
            else
            {
                return Tag2Pos<tTag, n+1>();
            }
        };

    public:
        using SublayerArray = typename SublayerPtrArray_<std::tuple<>, tSublayerTuple>::type;

    private:
        SublayerArray tuple_;

    public:
        /*
        * @brief：根据层的名称进行实例化
        * @param params：层构造函数所需要的参数
        */
        template<typenaem tTag, typename ...tParams>
        auto set(tParams &&...params)
        {
            constexpr static size_t pos = Tag2Pos<tTag, 0>();
            using LayerType = std::tuple_element_t<pos, tSublayerTuple>::Layer;
            std::get<pos>(tuple_) = std::make_shared<LayerType>(std::forward<tParams>(params)...);
            return *this;
        }

        // TODO:operator SublayerArray() 类型转换函数
    };

    template<size_t n, typename tSaver, typename tSublayerArray>
    void SaveWeightsHelp(tSaver &saver, tSublayerArray &sublayers)
    {
        if constexpr(n != ArraySize<tSublayerArray>)
        {
            auto layerPtr = std::get<n>(sublayers);
            LayerSaveWeights(*layerPtr, saver);
            SaveWeightsHelp<n+1>(saver, sublayers);
        }
    }
    
    template<size_t n, typename tGradCollector, typename tSublayerArray>
    void GradCollectHelp(tGradCollector &collector, tSublayerArray &sublayers)
    {
        if constexpr(n != ArraySize<tSublayerArray>)
        {
            auto layerPtr = std::get<n>(sublayers);
            LayerGradCollect(*layerPtr, collector);
            GradCollectHelp<n+1>(collector, sublayers);
        }
    }

    template<size_t n, typename tSublayerArray>
    void NeutralInvariantHelp(tSublayerArray &sublayers)
    {
        if constexpr(n != ArraySize<tSublayerArray>)
        {
            auto layerPtr = std::get<n>(sublayers);
            LayerNeutralInvariant(*layerPtr);
            NeutralInvariantHelp<n+1>(sublayers);
        }
    }

    template<size_t n, typename tLayerSpecialCont, typename tPolicyCont, typename tInitializerManager, typename tLoadBuffer, typename tSublayerArray>
    void InitHelp(tInitializerManager &paramInitializer, tLoadBuffer &loadBuffer, tSublayerArray &sublayers)
    { 
        if constexpr(n != ArraySize<tSublayerArray>)
        {
            using LayerInfo = std::tuple_element_t<n, tLayerSpecialCont>;
            auto layerPtr = std::get<n>(sublayers);
            using InitPolicy = SubPolicyPicker<tPolicyCont, typename LayerInfo::Tag>;

            LayerInit<typename LayerInfo::type, tInitializerManager, tLoadBuffer, InitPolicy>(*layerPtr, paramInitializer, loadBuffer);
            InitHelp<n+1, tLayerSpecialCont, tPolicyCont>(paramInitializer, loadBuffer, sublayers);
        }
    };

    template <typename tRes, typename tInput>
    struct InternalResult_
    {
        using type = tRes;
    };
    template <typename ...tProcessed, typename tCur, typename ...tRem>
    struct InternalResult_<VarTypeDict<tProcessed...>, std::tuple<tCur, tRem...>>
    {
        using type = typename InternalResult_<VarTypeDict<tProcessed..., typename tCur::Tag>, std::tuple<tRem...>>::type;
    };

    template<typename tLayerSpecialCont>
    using InternalResult = typename InternalResult_<VarTypeDict<>, tLayerSpecialCont>::type;

    template<typename tAimTag, typename tInConnectCont, typename tInput, typename tOutput>
    auto InputFromInConnect(const tInput &input, tOutput &&output)
    {
        if constexpr(0 == ArraySize<tInConnectCont>)
        {
            return std::forward<tOutput>(output);
        }
        else
        {
            using CurInConnect = SeqHead<tInConnectCont>;
            using TmpInConnectCont = SeqTail<tInConnectCont>;

            if constexpr(std::is_same<tAimTag, typename CurInConnect::InLayerTag>::value)
            {
                // TODO：std::forward<tOutput>(output)改为move(output)
                auto res = std::forward<tOutput>(output).template Set<typename CurInConnect::InLayerName>(input.template Get<typename CurInConnect::InName>());
                return InputFromInConnect<tAimTag, TmpInConnectCont>(input, mover(res));
            }
            else
            {
                return InputFromInConnect<tAimTag, TmpInConnectCont>(input, std::forward<tOutput>(output));
            }
        }
    }

    template<typename tAimTag, typename tInterConnectCont, typename tInternalInput, typename tOutput>
    auto InputFromInternalConnect(const tInternalInput &input, tOutput &&output)
    {      
        if constexpr(0 == ArraySize<tInConnectCont>)
        {
            return std::forward<tOutput>(output);
        }
        else
        {
            using CurInterConnect = SeqHead<tInterConnectCont>;
            using TmpInterConnectCont = SeqTail<tInterConnectCont>;

            if constexpr(std::is_same<tAimTag, typename CurInterConnect::InLayerTag>::value)
            {
                auto preLayerRes = input.template Get<typename CurInterConnect::OutLayerTag>();
                auto res = std::forward<tOutput>(output).template Set<typename CurInterConnect::InLayerName>(preLayerRes.template Get<typename CurInterConnect::OutLayerName>());
                return InputFromInternalConnect<tAimTag, TmpInterConnectCont>(input, mover(res));
            }
            else
            {
                return InputFromInternalConnect<tAimTag, TmpInterConnectCont>(input, std::forward<tOutput>(output));
            }
        }
        
    }

    template<typename tAimTag, typename tOutConnectCont, typename tLayerInput, typename tOutput>
    auto FillOutput(const tLayerInput &input, tOutput &&output)
    {
          if constexpr(0 == ArraySize<tOutConnectCont>)
        {
            return std::forward<tOutput>(output);
        }
        else
        {
            using CurOutConnect = SeqHead<tOutConnectCont>;
            using TmpOutConnectCont = SeqTail<tOutConnectCont>;

            if constexpr(std::is_same<tAimTag, typename CurOutConnect::OutLayerTag>::value)
            {
                auto tmp = input.template Get<typename CurOutConnect::OutLayerName>();
                auto res = std::forward<tOutput>(output).template Set<typename CurOutConnect::OutName>(move(tmp));
                return FillOutput<tAimTag, TmpOutConnectCont>(input, mover(res));
            }
            else
            {
                return FillOutput<tAimTag, TmpOutConnectCont>(input, std::forward<tOutput>(output));
            }
        }
    }

    template<size_t n, typename tInConnectCont, typename tInterConnectCont, typename tOutConnectCont, typename tLayerSpecialCont, typename tIn, typename tSublayerArray, typename tInternalRes, typename tOut>
    auto FeedForwarHelp(const tIn &in, tSublayerArray &sublayers, tInternalRes &&internalRes, tOut &&out)
    {
        if constexpr(n == ArraySize<tSublayerArray>)
        {
            return std::forward<tOut>(out);
        }
        else
        {
            auto &layer = *(std::get<n>(sublayers));
            using LayerType = std::decay_t<decltype(layer)>;
            using InputType = typename LayerType::InputType;

            using CurLayerTag = typename std::tuple_element_t<n, tLayerSpecialCont>::Tag;

            auto Input1 = InputFromInConnect<CurLayerTag, tInConnectCont>(in, InputType::Create());
            auto Input2 = InputFromInternalConnect<CurLayerTag, tInterConnectCont>(internalRes, move(Input1));
            auto res = layer.FeedForward(move(Input2));

            auto newOut = FillOutput<CurLayerTag, tOutConnectCont>(res, std::forward<tOut>(out));
            auto newInternalRes = std::forward<tInternalRes>(internalRes).template Set<CurLayerTag>(move(res));

            return  FeedForwarHelp<n+1, tInConnectCont, tInterConnectCont, tOutConnectCont, tLayerSpecialCont>(in, sublayers_, move(newInternalRes), move(newOut));
        }
    }

  /*
  * @brief
  *     1 子层对象管理
  *     2 参数保存 梯度收集 中性检测
  *     3 参数初始化
  *     4
  * @param tInputType：输入容器
  * @param tOutputType：输出容器
  * @param tPolicyCont：policy容器
  * @param tComposeTopology：struct ComposeTopology
  * 
  */
    template<typename tInputType, typename tOutputType, typename tPolicyCont, typename tComposeTopology>
    class ComposeKernel
    {
    private:

        using OutputType = tOutputType;

        // LayerSpecializationContainer的形式为tuple<SpecialSublayer<layername1, layer1<policies>>, SpecialSublayer<layername2, layer2<policies>>, SpecialSublayer<layername3, layer3<policies>>> 
        using LayerSpecializationContainer = typename tComposeTopology::template Specialization<tPolicyCont>;

        // 1 子层对象管理
        // SublayerArray 的形式为 tuple<shared_ptr<layer1<policies>>, shared_ptr<layer2<policies>>, shared_ptr<layer3<policies>>>
        using SublayerArray = typename SublayerArrayMaker<LayerSpecializationContainer>::SublayerArray;
        SublayerArray sublayers_;

    public:

        static auto CreateSublayers()
        {
            return  SublayerArrayMaker<LayerSpecializationContainer>();
        }

        // 2.1 参数保存
        template<typename tSaver>
        void SaveWeights(tSaver &saver)
        {
            SaveWeightsHelp<0>(saver, sublayers_);
        }

        // 2.2 梯度收集
        template<typename tGradCollector>
        void GradCollect(tGradCollector &collector)
        {
            GradCollectHelp<0>(collector, sublayers_);
        }

        // 2.3 中性检测
        void NeutralInvariant()
        {
            NeutralInvariantHelp<0>(sublayers_);
        }

        // 3 参数初始化
        template<typename tInitializerManager, typename tLoadBuffer, typenaem tPolicies = typename tInitializerManager::PolicyCont>
        void Init(tInitializerManager &paramInitializer, tLoadBuffer &loadBuffer)
        {
            InitHelp<0, LayerSpecializationContainer, tPolicies>(paramInitializer, loadBuffer, sublayers_);
        }

        // 4.1 正向传播 
        template <typename T>
        auto FeedForward(const T &in)
        {
            using InternalResType = InternalResult<LayerSpecializationContainer>;
            return FeedForwarHelp<0, typename tComposeTopology::InputConnects, typename tComposeTopology::InterConnects, typename tComposeTopology::OutputConnects, LayerSpecializationContainer>(in, sublayers_, InternalResType::Create(), OutputType::Create());
        }

    };

}