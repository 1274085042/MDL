#pragma once

#include "tags.h"
#include <iterator>
#include <type_traits>

namespace MDL
{
	template<typename tElem, typename tDevice>
	class Scalar;

	template<typename tElem, typename tDevice>
	class Matrix;

	template<typename tElem, typename tDevice, typename tCategory>
	class Batch;

	//通过类别标签、计算单元与计算设备确定主体类型
	template <typename tCategory, typename tElem, typename tDevice>
	struct PrincipalDataType_;
	template<typename tElem, typename tDevice>
	struct PrincipalDataType_<CategoryTags::Scalar, tElem, tDevice>
	{
		using type = Scalar<tElem, tDevice>;
	};
	template<typename tElem, typename tDevice>
	struct PrincipalDataType_<CategoryTags::Matrix, tElem, tDevice>
	{
		using type = Matrix<tElem, tDevice>;
	};
	template<typename tElem, typename tDevice>
	struct PrincipalDataType_<CategoryTags::ScalarBatch, tElem, tDevice>
	{
		using type = Batch<tElem, tDevice, CategoryTags::ScalarBatch>;
	};	
	template<typename tElem, typename tDevice>
	struct PrincipalDataType_<CategoryTags::MatrixBatch, tElem, tDevice>
	{
		using type = Batch<tElem, tDevice, CategoryTags::MatrixBatch>;
	};
	template<typename tCategory, typename tElem, typename tDevice>
	using PrincipalDataType = typename PrincipalDataType_<tCategory, tElem, tDevice>::type;

	template<typename T>
	constexpr bool IsScalar = false;
	template<typename T>
	constexpr bool IsScalar<const T> = IsScalar<T>;
	template<typename T>
	constexpr bool IsScalar<T &> = IsScalar<T>;
	template<typename T>
	constexpr bool IsScalar<T &&> = IsScalar<T>;

	template<typename T>
	constexpr bool IsMatrix = false;
	template<typename T>
	constexpr bool IsMatrix<const T> = IsMatrix<T>;
	template<typename T>
	constexpr bool IsMatrix<T&> = IsMatrix<T>;
	template<typename T>
	constexpr bool IsMatrix<T&&> = IsMatrix<T>;
	
	template<typename T>
	constexpr bool IsScalarBatch = false;
	template<typename T>
	constexpr bool IsScalarBatch<const T> = IsScalar<T>;
	template<typename T>
	constexpr bool IsScalarBatch<T&> = IsScalar<T>;
	template<typename T>
	constexpr bool IsScalarBatch<T&&> = IsScalar<T>;

	template<typename T>
	constexpr bool IsMatrixBatch = false;
	template<typename T>
	constexpr bool IsMatrixBatch<const T> = IsMatrixBatch<T>;
	template<typename T>
	constexpr bool IsMatrixBatch<T&> = IsMatrixBatch<T>;
	template<typename T>
	constexpr bool IsMatrixBatch<T&&> = IsMatrixBatch<T>;

	//DataCategory_根据具体类型返回类别
	//如果T为Matrix<int, CPU>，则type为CategoryTags::Matrix
	template <typename T>
	struct DataCategory_
	{
		template <bool bIsScalar, bool bIsMatrix, bool bIsScalarBatch, bool bIsMatrixBatch, typename tDummy=void>
		struct helper;
		template <typename tDummy >
		struct helper<true, false, false, false, tDummy>
		{
			using type = CategoryTags::Scalar;
		};
		template <typename tDummy >
		struct helper<false, true, false, false, tDummy>
		{
			using type = CategoryTags::Matrix;
		};
		template <typename tDummy >
		struct helper<false, false, true, false, tDummy>
		{
			using type = CategoryTags::ScalarBatch;
		};
		template <typename tDummy >
		struct helper<false, false, false, true, tDummy>
		{
			using type = CategoryTags::MatrixBatch;
		};

		using type =typename helper<IsScalar<T>, IsMatrix<T>, IsScalarBatch<T>, IsMatrixBatch<T>>::type;
	};
	template <typename T>
	using DataCategory =typename DataCategory_<T>::type;

	template <typename T>
	using RemConstRef = std::remove_cv_t<std::remove_reference_t<T>>;

	template <typename T>
	struct IsIterator_
	{
		//如果tIterator是迭代器，则iterator_traits<tIterator>::iterator_category是存在的，其值是迭代器类别，编译器选择第一个Test声明
		//如果tIterator不是迭代器，Test<T>(nullptr)将无法匹配第一个Test，根据匹配失败并非错误的原则，编译器选择第二个Test声明
		template <typename tIterator>
		static std::true_type Test(typename std::iterator_traits<tIterator>::iterator_category*);
		template <typename tIterator>
		static std::false_type Test(...); //参数为...的函数匹配程度最低，

		//decltype(Test<T>(nullptr))将返回表达式的类型
		static constexpr bool value = decltype(Test<T>(nullptr))::value;
	};
	template <typename T>
	constexpr bool IsIterator=IsIterator_<T>::value;

}