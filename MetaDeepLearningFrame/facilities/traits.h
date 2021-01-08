#pragma once

//#include <cstddef>
#include<type_traits>

namespace MDL
{ 
	template<bool b, typename T>
	static constexpr bool AddValue = false;
	template<typename T>
	static constexpr bool AddValue<true, T> = T::value;
	
	//判断容器是否为空
	template <typename T>
	struct IsContainerEmpty;
	template <template <typename ... > class tCont, typename T1, typename ...T2>
	struct IsContainerEmpty<tCont<T1, T2...>>
	{
		static constexpr bool value = false;
	};
	template <template <typename ... > class tCont>
	struct IsContainerEmpty<tCont<>>
	{
		static constexpr bool value = true;
	};

	template <typename T>
	static constexpr bool DependencyFalse = false;

	template <typename T>
	using RemConstRef = std::remove_cv_t<std::remove_reference_t<T>>;
}
