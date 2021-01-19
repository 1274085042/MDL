#pragma once

//#include "matrices.h"
#include "../facilities/traits.h"
#include <cstddef>
#include <utility>

namespace MDL
{
	template<typename tElem, typename tDevice, typename tScalar>
	class TrivalMatrix
	{
	public:
		using ElementType = tElem;
		using DeviceType = tDevice;

	private:
		tScalar val_;
		size_t rowNum_;
		size_t colNum_;
		//EvalBuffer<Matrix<ElementType, DeviceType>> evalBuf_  //evalBuf_用于保存求值结果

	public:
		TrivalMatrix(size_t rowNum, size_t colNum, tScalar val):val_(val), rowNum_(rowNum), colNum_(colNum)
		{}

		bool operator==(const TrivalMatrix& tMatrix) const
		{
			return (val_ == tMatrix.val_) && (rowNum_ == tMatrix.rowNum_) && (colNum_ == tMatrix.colNum_);
		}

		template <typename tOtherType>
		bool operator==(const tOtherType&) const
		{
			return false;
		}

		template <typename T>
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

		auto ElementValue() const	//访问矩阵中的标量值
		{
			return val_;
		}

		auto EvalRegister() const
		{

		}
	};


	template <typename tElem, typename tDevice, typename tScalar>
	constexpr bool IsMatrix<TrivalMatrix<tElem, tDevice, tScalar>> = true;

	//MakeTrivalMatrix用于简化TrivalMatrix的构造	
	template <typename tElem, typename tDevice, typename tType>
	auto MakeTrivalMatrix(size_t rowNum, size_t colNum, tType&& val)
	{
		using RawType = RemConstRef<tType>;
		if constexpr (IsScalar<RawType>)	//如果是标量类型
		{
			//标量的设备类型是CPU或者与TrivalMatrix的计算设备类型相同，
			//这表明可以在CPU端构造平凡矩阵，或者在相同的计算设备类型中使用标量来构造平凡矩阵
			static_assert(std::is_same<typename RawType::DeviceType, tDevice>::value || std::is_same<typename RawType::DeviceType, DeviceTags::CPU>::value);
			return TrivalMatrix<tElem, tDevice, RawType>(rowNum, colNum, std::forward<tType>(val));
		}
		else   //如果是数值类型，先使用数值构造一个设备类型为CPU的标量，使用构造好的标量再进一步构造平凡矩阵
		{
			tElem tmp = static_cast<tElem> (val);
			Scalar<tElem, DeviceTags::CPU> scalar(std::move(tmp));
			return TrivalMatrix<tElem, tDevice, Scalar<tElem, DeviceTags::CPU>>(rowNum, colNum, std::move(scalar));
		}
	}
}