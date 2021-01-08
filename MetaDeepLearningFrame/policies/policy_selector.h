#pragma once

#include "traits.h"
#include "policy_container.h"
#include <type_traits>

namespace MDL
{
	namespace NSPolicySelect
	{
		//将属于tMajorClass的policy放到容器tPolicyContainer中
		//也就是将属于同一组的policy放入容器中
		template<typename tMajorClass, typename tPolicyContainer, typename ...T>
		struct MajorFilter_				//当 ...T为空时，递归终止
		{
			using type = tPolicyContainer;
		};
		template<typename tMajorClass, typename ...tFilteredPolicies, typename tCurPolicy, typename ...T>
		struct MajorFilter_<tMajorClass, PolicyContainer<tFilteredPolicies...>, tCurPolicy, T...>
		{
			template<typename tCurMajorClass, typename tDummy=void>		
			struct impl_				//不属于tMajorClass的policy丢掉
			{
				using type =typename MajorFilter_<tMajorClass, PolicyContainer<tFilteredPolicies...>, T...>::type;
			};
			template<typename tDummy>		   //因为template的模板参数不能为空（template< >）,因此引入tDummy，同时在原始模板中将tDummy设置一个默认值void
			struct impl_<tMajorClass, tDummy>  //属于tMajorClass的policy放入容器
			{
				using type =typename MajorFilter_<tMajorClass, PolicyContainer<tFilteredPolicies..., tCurPolicy>, T...>::type;
			};
			using type = typename impl_<typename tCurPolicy::MajorClass>::type;
		};

		/*
		* 检查PolicyContainer<policies...>中的所有policy是否有冲突
		（也就是检查属于同一组的任意两个policy的MinorPolicy是否相同）
		* 外层循环MinorCheck_
		* 内层循环MinorDep_
		*/
		template<typename tMinorClass, typename ...T>
		struct MinorDep_
		{
			static constexpr bool value = true;
		};
		template<typename tMinorClass, typename tCurPolicy, typename ...T>
		struct MinorDep_<tMinorClass,tCurPolicy, T...>
		{
			using tCurMinorClass =typename tCurPolicy::MinorClass;
			static constexpr bool cur_check = !(std::is_same<tMinorClass, tCurMinorClass>::value);
			static constexpr bool value = AddValue<cur_check, MinorDep_<tMinorClass, T...>>;
		};

		template<typename tPolicyContainer>
		struct MinorCheck_			
		{
			static constexpr bool value = true;
		};
		template<typename tCurPolicy, typename ...T>
		struct MinorCheck_<PolicyContainer<tCurPolicy, T...>>
		{
			static constexpr bool cur_value=MinorDep_<typename tCurPolicy::MinorClass, T...>::value;
			static constexpr bool value = AddValue<cur_value, MinorCheck_<PolicyContainer<T...>>>;
		};

		template<typename tPolicyCont>
		struct PolicySelRes;
		template<typename tPolicy>
		struct PolicySelRes<PolicyContainer<tPolicy>> :public tPolicy
		{};
		template<typename tCurPolicy, typename ...tOtherPolicies>
		struct  PolicySelRes<PolicyContainer<tCurPolicy, tOtherPolicies...>>:public tCurPolicy , public PolicySelRes<PolicyContainer<tOtherPolicies...>>
		{};

		/*-------------------------------------------------------------------------------------------*/
		template <typename tMajorClass, typename tPolicyContainer>
		struct  Selector_;
		template <typename tMajorClass, typename ...tPolicies>
		struct Selector_<tMajorClass, PolicyContainer<tPolicies...>>
		{
			using PC = typename MajorFilter_<tMajorClass, PolicyContainer<>, tPolicies...>::type;
			static_assert(MinorCheck_<PC>::value, "Minor class set conflict!");   //static_assert(p,"")编译期静态断言,当第一个参数是false时，提供后面的文本

			using type = std::conditional_t<IsContainerEmpty<PC>::value, tMajorClass, PolicySelRes<PC>>;
		};
	}

	template <typename tMajorClass, typename tPolicyContainer>
	using PolicySelect = typename NSPolicySelect::Selector_<tMajorClass, tPolicyContainer>::type;
}