#pragma once

#include "../facilities/tags.h"
#include "../facilities/traits.h"

namespace MDL
{
	//template <typename tElem, typename tDevice, typename tCategory>
	//class Batch;

	template <typename tElem, typename tDevice>
	constexpr bool IsScalarBatch<Batch<tElem, tDevice, CategoryTags::Scalar>> = true;
	template <typename tElem, typename tDevice>
	constexpr bool IsScalarBatch<Batch<tElem, tDevice, CategoryTags::Scalar> &> = true;
	template <typename tElem, typename tDevice>
	constexpr bool IsScalarBatch<Batch<tElem, tDevice, CategoryTags::Scalar> &&> = true;
	template <typename tElem, typename tDevice>
	constexpr bool IsScalarBatch<const Batch<tElem, tDevice, CategoryTags::Scalar>> = true;
	template <typename tElem, typename tDevice>
	constexpr bool IsScalarBatch<const Batch<tElem, tDevice, CategoryTags::Scalar> &> = true;
	template <typename tElem, typename tDevice>
	constexpr bool IsScalarBatch<const Batch<tElem, tDevice, CategoryTags::Scalar> &&> = true;
	
	template <typename tElem, typename tDevice>
	constexpr bool IsMatrixBatch<Batch<tElem, tDevice, CategoryTags::Matrix>> = true;
}