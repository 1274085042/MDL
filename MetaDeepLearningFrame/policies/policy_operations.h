#pragma once

#include "policy_container.h"
#include <functional>

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
        using type = NewPlainPolicy::type;
    };

    template <typename tPolicyContainer>
    using PlainPolicy = PlainPolicy_<tPolicyContainer, PolicyContainer<>>::type;
}