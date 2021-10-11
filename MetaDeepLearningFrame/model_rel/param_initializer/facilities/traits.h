#pragma once

#include "policies.h"
#include "../../../facilities/traits.h"
#include "../../../policies/policy_selector.h"
#include "../../../facilities/var_type_dict.h"
#include "../../../policies/policy_container.h"
#include "../../../policies/policy_operations.h"

#include <tuple>
#include <utility>

namespace MDL
{
    template<typename TRes, typename ...TPolicies>
    struct FillerTagFromPolicies_
    {
        using type = TRes;
    };
    template<typename ...TRes, typename TCur, typename ...TRem>
    struct FillerTagFromPolicies_<std::tuple<TRes...>, PInitializerIs<TCur>, TRem...>
    {
        using type = typename FillerTagFromPolicies_<std::tuple<TRes..., TCur>, TRem...>::type;
    };
    template<typename ...TRes, typename TCur, typename ...TRem>
    struct FillerTagFromPolicies_<std::tuple<TRes...>, PWeightInitializerIs<TCur>, TRem...>
    {
        using type = typename FillerTagFromPolicies_<std::tuple<TRes..., TCur>, TRem...>::type;
    };
    template<typename ...TRes, typename TCur, typename ...TRem>
    struct FillerTagFromPolicies_<std::tuple<TRes...>, PBiasInitializerIs<TCur>, TRem...>
    {
        using type = typename FillerTagFromPolicies_<std::tuple<TRes..., TCur>, TRem...>::type;
    };
    template<typename ...TRes, typename TLayer, typename ...TSub, typename ...TRem>
    struct FillerTagFromPolicies_<std::tuple<TRes...>, SubPolicyContainer<TLayer, TSub...>, TRem...>
    {
        //using step1 = typename FillerTagFromPolicies_<std::tuple<TRes...>, TSub...>::type;
        //using type = typename FillerTagFromPolicies_<std::tuple<TRes..., TCur>, TRem...>::type;

        using type = typename FillerTagFromPolicies_<std::tuple<TRes...>, TSub..., TRem...>::type;
    };

    template <typename TCheck, typename ...TCheckes>
    struct AlreadyExist_
    {
        static constexpr bool value = false;
    };
    template <typename TCheck, typename TCur, typename ...TRem>
    struct AlreadyExist_<TCheck, TCur, TRem...>
    {
        static constexpr bool value = OrValue<std::is_same<TCheck, TCur>::value, AlreadyExist_<TCheck, TRem...>>;
    };

    template<typename TRes, typename TPolicyTuple>
    struct TagDedupe_
    {
        using type = TRes;
    };
    template<typename ...TRes, typename TCur, typename ...TRem>
    struct TagDedupe_<VarTypeDict<TRes...>, std::tuple<TCur, TRem...>>
    {

        using type = typename std::conditional_t<AlreadyExist_<TCur, TRem...>::value, 
                                        TagDedupe_<VarTypeDict<TRes...>, std::tuple<TRem...>>,
                                        TagDedupe_<VarTypeDict<TRes..., TCur>, std::tuple<TRem...>>>::type;
    };

    template<typename ...TPolicies>
    struct FillerTags2NamedParams_
    {
        using step1 = typename FillerTagFromPolicies_<std::tuple<>, TPolicies...>::type;
        using type = typename TagDedupe_<VarTypeDict<>, step1>::type;
    };

    /*
    * FillerTags2NamedParams 等于 VarTypeDict<tags...>
    * 第一步 FillerTagFromPolicies_得到tuple<tags...>
    * 第二步 TagDedupe_对tuple<tags...>里面重复的tag进行过滤，过滤的tag放入VarTypeDict<>
    */
    template<typename ...TPolicies>
    using FillerTags2NamedParams = typename FillerTags2NamedParams_<TPolicies ...>::type;


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
