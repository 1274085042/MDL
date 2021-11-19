#pragma once
#include "policy_container.h"

namespace MDL
{   
    template <typename tNewPolicy, typename tOriContainer>
    struct ChangePolicy_;
    template <typename tNewPolicy, typename ...tPolicies>
    struct ChangePolicy_<tNewPolicy, PolicyContainer<tPolicies...>>
    {
    private:
        using NewMajorClass = typename tNewPolicy::MajorClass;
        using NewMinorClass = typename tNewPolicy::MinorClass;

        template<typename tDropContainer, typename ...tP>
        struct DropAppend_;
        template<typename ...tFilterPolicies>
        struct DropAppend_<PolicyContainer<tFilterPolicies...>>
        {
            using type = PolicyContainer<tFilterPolicies..., tNewPolicy>;
        };
        template<typename ...tFilterPolicies, typename tCurPolicy, typename ...tP>
        struct DropAppend_<PolicyContainer<tFilterPolicies...>, tCurPolicy, tP...>
        {
        private:
            template<bool b, typename tDummy=void>
            struct ArrayBasedSwitch_
            {
                template <typename tMajorClass, typename tMinorClass, typename tPlaceholder = void>
                struct Impl_
                {
                    using type = PolicyContainer<tFilterPolicies..., tCurPolicy>;
                };
                template<typename tPlaceholder>
                struct Impl_<NewMajorClass, NewMinorClass, tPlaceholder>
                {
                    using type = PolicyContainer<tFilterPolicies...>;
                };
            
                using type = typename Impl_<typename tCurPolicy::MajorClass, typename tCurPolicy::MinorClass>::type;
            };

            template<typename tDummy>
            struct ArrayBasedSwitch_<true, tDummy>
            {
                using type = PolicyContainer<tFilterPolicies..., tCurPolicy>;
            };

        public:
            using tmp = typename ArrayBasedSwitch_<IsSubPolicyContainer<tCurPolicy>>::type;
            using type = typename DropAppend_<tmp, tP...>::type;
        };

    public:
        using type = typename DropAppend_<PolicyContainer<>, tPolicies...>::type;
    };

    template <typename tNewPolicy, typename tOriContainer>
    using ChangePolicy = typename ChangePolicy_<tNewPolicy, tOriContainer>::type;
}
