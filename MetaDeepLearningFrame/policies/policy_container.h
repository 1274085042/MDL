#pragma once

namespace MDL
{
	template <typename ...tPolicies>
	struct PolicyContainer;		//容器模板

	template <typename T>
	constexpr bool IsPolicyContainer = false;

	template <typename ...tPolicies>
	constexpr bool IsPolicyContainer<PolicyContainer<tPolicies...>> = true;

	template<typename TLayerName, typename ...Tpolicies>
	struct SubPolicyContainer;

	template <typename T>
	constexpr bool IsSubPolicyContainer = false;

	template <typename L, typename ...tPolicies>
	constexpr bool IsSubPolicyContainer<SubPolicyContainer<L, tPolicies...>> = true;
}