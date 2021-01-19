#pragma once

#include "../facilities/traits.h"
#include <cstddef>

namespace MDL
{
	template<typename tElem, typename tDevice>
	class ZeroMatrix
	{
	public:
		using ElementType = tElem;
		using DeviceType = tDevice;

	private:
		size_t rowNum_;
		size_t colNum_;
		//EvalBuffer<Matrix<ElementType, DeviceType>> evalBuf_;

	public:
		ZeroMatrix(size_t rowNum, size_t colNum) :rowNum_(rowNum), colNum_(colNum)
		{}

		bool operator==(const ZeroMatrix& zMatrix) const
		{
			return (rowNum_ == zMatrix.rowNum_) && (colNum_ == zMatrix.colNum_);
		}

		template <typename tOtherType>
		bool operator==(const tOtherType&) const
		{
			return false;
		}

		template<typename T>
		bool operator!=(const T& tMatrix) const
		{
			return !(operator==(tMatrix));
		}

		size_t RowNum() const
		{
			return rowNum_;
		}

		size_t ColNum() const
		{
			return colNum_;
		}

		auto EvalRegister()  const
		{}
	};

	template <typename tElem, typename tDevice>
	constexpr bool IsMatrix<ZeroMatrix<tElem, tDevice>> = true;
}