#pragma once

#include "tags.h"
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

	template <typename T>
	using RemConstRef = std::remove_cv_t<std::remove_reference_t<T>>;
}