#pragma once

//#include <cstddef>
#include<type_traits>

namespace MDL
{ 
	template<bool b, typename T>
	static constexpr bool AndValue = false;
	template<typename T>
	static constexpr bool AndValue<true, T> = T::value;

	template<bool b, typename T>
	static constexpr bool OrValue = true;
	template<typename T>
	static constexpr bool OrValue<false, T> = T::value;
	
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

	template <typename tArray>
	struct ArraySize_;
	template<template<typename ...> class tArray, typename ...T>
	struct ArraySize_<tArray<T...>>
	{
		static constexpr int size = sizeof...(T); 
	};
	template<typename tArray>
	static constexpr int ArraySize = ArraySize_<tArray>::size; 

	template<typename T>
    struct Identity_
    {
        using type = T;
    };

	template<typename tCon>
	struct SeqHead_;
	template<template <typename ...> class tCon, typename tHead, typename ...tRemain>
	struct SeqHead_<tCon<tHead, tRemain...>>
	{
		using type = tHead;
	};

	template<typename tCon>
	using SeqHead = typename SeqHead_<tCon>::type;

	template<typename tCon>
	struct SeqTail_;
	template<template <typename ...> class tCon, typename tHead, typename ...tRemain>
	struct SeqTail_<tCon<tHead, tRemain...>>
	{
		using type = tCon<tRemain...>;
	};

	template<typename tCon>
	using SeqTail = typename SeqTail_<tCon>::type;

}
