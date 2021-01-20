#pragma once

#include "../facilities/traits.h"
#include <cassert>

namespace MDL
{
	template<typename tElem, typename tDevice>
	class OneHotVector
	{
	public:
		using ElementType = tElem;
		using DeviceType = tDevice; 

	private:
		size_t colNum_;
		size_t hotPos_;
		//EvalBuffer<Matrix<ElementType, DeviceType>> evalBuf_;

	public:
		OneHotVector(size_t colNum, size_t hotPos) :colNum_(colNum), hotPos_(hotPos)
		{
			assert(hotPos_ < colNum_);
		}

		bool operator==(const OneHotVector& oVector) const
		{
			return (colNum_ == oVector.colNum_) && (hotPos_ == oVector.hotPos_);
		}

		template <typename tOtherType>
		bool operator==(const tOtherType&) const
		{
			return false;
		}

		template <typename T>
		bool operator!=(const T& tVector) const
		{
			return !(operator==(tVector));
		}

		size_t RowNum() const
		{
			return 1;
		}

		size_t ColNum() const
		{
			return colNum_;
		}

		size_t HosPos() const
		{
			return hotPos_;
		}

		auto EvalRegister() const
		{}
	};

	template<typename tElem, typename tDevice>
	constexpr bool IsMatrix<OneHotVector<tElem, tDevice>> = true;
}