#pragma once

#include "./policies.h"
#include "../../../facilities/traits.h"
#include "../../../policies/policy_selector.h"
#include "../../../facilities/var_type_dict.h"
#include "../../../policies/policy_container.h"
#include "../../../policies/policy_operations.h"

#include <tuple>
#include <utility>

namespace MDL
{
    template<typename tRes, typename ...tPolicies>
    struct FillerTagFromPolicies_
    {
        using type = tRes;
    };
    template<typename ...tRes, typename tCur, typename ...tRem>
    struct FillerTagFromPolicies_<std::tuple<tRes...>, PInitializerIs<tCur>, tRem...>
    {
        using type = typename FillerTagFromPolicies_<std::tuple<tRes..., tCur>, tRem...>::type;
    };
    template<typename ...tRes, typename tCur, typename ...tRem>
    struct FillerTagFromPolicies_<std::tuple<tRes...>, PWeightInitializerIs<tCur>, tRem...>
    {
        using type = typename FillerTagFromPolicies_<std::tuple<tRes..., tCur>, tRem...>::type;
    };
    template<typename ...tRes, typename tCur, typename ...tRem>
    struct FillerTagFromPolicies_<std::tuple<tRes...>, PBiasInitializerIs<tCur>, tRem...>
    {
        using type = typename FillerTagFromPolicies_<std::tuple<tRes..., tCur>, tRem...>::type;
    };
    template<typename ...tRes, typename tLayer, typename ...tSub, typename ...tRem>
    struct FillerTagFromPolicies_<std::tuple<tRes...>, SubPolicyContainer<tLayer, tSub...>, tRem...>
    {
        //using step1 = typename FillerTagFromPolicies_<std::tuple<tRes...>, tSub...>::type;
        //using type = typename FillerTagFromPolicies_<step1, tRem...>::type;

        using type = typename FillerTagFromPolicies_<std::tuple<tRes...>, tSub..., tRem...>::type;
    };

    template <typename tCheck, typename ...tCheckes>
    struct AlreadyExist_
    {
        static constexpr bool value = false;
    };
    template <typename tCheck, typename tCur, typename ...tRem>
    struct AlreadyExist_<tCheck, tCur, tRem...>
    {
        static constexpr bool value = OrValue<std::is_same<tCheck, tCur>::value, AlreadyExist_<tCheck, tRem...>>;
    };

    template<typename tRes, typename tPolicyTuple>
    struct TagDedupe_
    {
        using type = tRes;
    };
    template<typename ...tRes, typename tCur, typename ...tRem>
    struct TagDedupe_<VarTypeDict<tRes...>, std::tuple<tCur, tRem...>>
    {

        using type = typename std::conditional_t<AlreadyExist_<tCur, tRem...>::value, 
                                        TagDedupe_<VarTypeDict<tRes...>, std::tuple<tRem...>>,
                                        TagDedupe_<VarTypeDict<tRes..., tCur>, std::tuple<tRem...>>>::type;
    };

    template<typename ...tPolicies>
    struct FillerTags2NamedParams_
    {
        using step1 = typename FillerTagFromPolicies_<std::tuple<>, tPolicies...>::type;
        using type = typename TagDedupe_<VarTypeDict<>, step1>::type;
    };

    /*
    * FillerTags2NamedParams 等于 VarTypeDict<tags...>
    * 第一步 FillerTagFromPolicies_得到tuple<tags...>
    * 第二步 TagDedupe_对tuple<tags...>里面重复的tag进行过滤，过滤的tag放入VarTypeDict<>
    */
    template<typename ...tPolicies>
    using FillerTags2NamedParams = typename FillerTags2NamedParams_<tPolicies...>::type;


    template <typename tPolicyCont, typename tGroup>
    struct Group2Initializer_;
    template <typename tPolicyCont>
    struct Group2Initializer_<tPolicyCont, InitPolicy::WeightTypeCate>
    {
      using type = typename PolicySelect<InitPolicy, tPolicyCont>::Weight;  
    };
    template <typename tPolicyCont>
    struct Group2Initializer_<tPolicyCont, InitPolicy::BiasTypeCate>
    {
      using type = typename PolicySelect<InitPolicy, tPolicyCont>::Bias;  
    };

    template <typename tPolicyCont, typename tSpecInit>
    struct PickInitializerBySpec_
    {
        using type = tSpecInit;
    };
    template <typename tPolicyCont>
    struct PickInitializerBySpec_<tPolicyCont, void>
    {
        using type = typename PolicySelect<InitPolicy, tPolicyCont>::Overall;
    };

    template <typename tPolicyCont, typename tSpecInit>
    struct PickInitializer_
    {
        static_assert(IsPolicyContainer<tPolicyCont>);
        using CurPolicyCont = PlainPolicy<tPolicyCont>;

        static_assert(!std::is_same<tSpecInit, InitPolicy::OverallTypeCate>::value);

        using SpecInitializer = typename Group2Initializer_<CurPolicyCont, tSpecInit>::type;

        using type = typename PickInitializerBySpec_<CurPolicyCont, SpecInitializer>::type;
    };

    /*
    * PickInitializer获取初始化器标签
    * 如果标签为void，则使用默认类型Overall
    */
    template <typename tPolicyCont, typename tSpecInit>
    using PickInitializer = typename PickInitializer_<tPolicyCont, tSpecInit>::type;
}
