#pragma once

#include "policy_container.h"
#include "../facilities/traits.h"
#include <functional>
#include <type_traits>

namespace MDL
{
    template <typename tFilteredPolicyContainer, typename tResPolicyContainer>
    struct PlainPolicy_
    {
        using type = tResPolicyContainer;
    };
    template <typename tCurPolicy, typename ...tPolicies, typename ...tFilteredPolicies>
    struct PlainPolicy_<PolicyContainer<tCurPolicy, tPolicies...>, PolicyContainer<tFilteredPolicies...>>
    {
        using NewPlainPolicy = std::conditional_t<IsSubPolicyContainer<tCurPolicy>,
                                PlainPolicy_<PolicyContainer<tPolicies...>, PolicyContainer<tFilteredPolicies...>>,
                                PlainPolicy_<PolicyContainer<tPolicies...>, PolicyContainer<tFilteredPolicies..., tCurPolicy>>>;
        using type = typename NewPlainPolicy::type;
    };
    template <typename tPolicyContainer>
    using PlainPolicy = typename PlainPolicy_<tPolicyContainer, PolicyContainer<>>::type;

    template <typename tPolicyContainer, typename tLayerName>
    struct SPP_;
    template < typename tLayerName>
    struct SPP_<PolicyContainer<>, tLayerName>
    {
        using type = PolicyContainer<>;
    };
    template <typename tCur, typename ...tPolicies, typename tLayerName>
    struct SPP_<PolicyContainer<tCur, tPolicies...>, tLayerName>
    {
        using type = typename SPP_<PolicyContainer<tPolicies...>, tLayerName>::type;
    };
    template <typename ...tCur,typename ...tPolicies, typename tLayerName>
    struct SPP_<PolicyContainer<SubPolicyContainer<tLayerName, tCur...> , tPolicies...>, tLayerName>
    {
        using type = PolicyContainer<tCur...>;
    };

    template <typename tPolicy, typename tSubPolicyContainer>
    struct PolicyExist_
    {
        constexpr static bool value = false;
    };
    template <typename tPolicy, typename tCur, typename ...tPolicies>
    struct PolicyExist_<tPolicy, PolicyContainer<tCur, tPolicies...>>
    {
        using MJ1 = typename tPolicy::MajorClass;
        using MJ2 = typename tCur::MajorClass;
        using MI1 = typename tPolicy::MinorClass;
        using MI2 = typename tCur::MinorClass;

        constexpr static bool tmp1 = std::is_same<MJ1, MJ2>::value;
        constexpr static bool tmp2 = AndValue<tmp1, std::is_same<MI1, MI2>>;
        constexpr static bool value = OrValue<tmp2, PolicyExist_<tPolicy, PolicyContainer<tPolicies...>>>;
    };
    template <typename tPolicy, typename tLayerName, typename ...T1, typename ...T2>
    struct PolicyExist_<tPolicy, PolicyContainer<SubPolicyContainer<tLayerName, T1...>, T2...>>
    {
        constexpr static bool value = PolicyExist_<tPolicy, PolicyContainer<T2...>>::value;
    };

    template <typename tParaentPolicyContainer, typename tFilteredContainer, typename tSubPolicyContainer>
    struct PolicyDeriveFil_
    {
        using type = tFilteredContainer;
    };
    template <typename tCur,typename ...tProcessing,typename ...tFilteredPolicies, typename tSubPolicyContainer>
    struct PolicyDeriveFil_<PolicyContainer<tCur, tProcessing...>, PolicyContainer<tFilteredPolicies...>, tSubPolicyContainer>
    {
        constexpr static bool duple = PolicyExist_<tCur, tSubPolicyContainer>::value;
        using tmp = std::conditional_t<duple, PolicyContainer<tFilteredPolicies...>,  PolicyContainer<tFilteredPolicies..., tCur>>;
        using type = typename PolicyDeriveFil_<PolicyContainer<tProcessing...>, tmp, tSubPolicyContainer>::type;
    };

    template <typename tFilteredContainer, typename tSubPolicyContainer>
    struct PolicyCasCade_;
    template <typename ...tParPolicies, typename ...tSubPolicies>
    struct PolicyCasCade_<PolicyContainer<tParPolicies ...>, PolicyContainer<tSubPolicies ...>>
    {
        using type = PolicyContainer<tParPolicies ..., tSubPolicies ...>;
    };
    template<typename tParaentPolicyContainer, typename tSubPolicyContainer>
    struct PolicyDerive_
    {
        using NewFilteredContainer = typename PolicyDeriveFil_<tParaentPolicyContainer, PolicyContainer<>, tSubPolicyContainer>::type;
        using type =typename PolicyCasCade_<NewFilteredContainer, tSubPolicyContainer>::type;
    };

    template<typename tPolicyContainer, typename tLayerName>
    struct SubPolicyPicker_
    {
        using tmp1 = typename SPP_<tPolicyContainer, tLayerName>::type;
        using tmp2 = PlainPolicy<tPolicyContainer>;
        using type = typename PolicyDerive_<tmp2, tmp1>::type;
    };
    template<typename tLayerName>
    struct SubPolicyPicker_<PolicyContainer<>, tLayerName>
    {
        using type = PolicyContainer<>;
    };

    /*
    * SubPolicyPicker方案：
    * 遍历PolicyContainer中的policy，如果存在一个SubPolicyContainer，其第一个模板参数和要找的子层名称相同，那么构造PolicyContainer容器并返回
    * 如果不存在这样的SubPolicyContainer，则返回复合层的policy。
    * 然后构造一个新的容器，构造新容器时要考虑原有的PolicyContainer中的policy和SubPolicyContainer中的policy是否相容
    * 如果相容，则将PolicyContainer中的policy放入新容器
    * 否则，跳过PolicyContainer中的policy
    *
    * 应用：
    * using PC = PolicyContainer<P1, SubPolicyContainer<SubX, P2, SubPolicyContainer<SubY, P3>>>
    * 调用SubPolicyPicker<PC, SubX>的结果
    *   - 如果P1与P2相容
    *     PolicyContainer<P1, P2, SubPolicyContainer<SubY, P3>>
    *   - 如果P1与P2互斥
    *     PolicyContainer<P2, SubPolicyContainer<SubY, P3>>
    */
    template<typename tPolicyContainer, typename tLayerName>
    using SubPolicyPicker = typename SubPolicyPicker_<tPolicyContainer, tLayerName>::type;
}