#pragma once

#include "matrices.h"
#include "../facilities/tags.h"
#include "../facilities/lower_access.h"
#include "../facilities/continuous_memory.h"
#include <cassert>

namespace MDL
{
	template <typename tElem>
	struct LowerAccessImpl<Matrix<tElem, DeviceTags::CPU>>;

	template <typename tElem>
	class Matrix<tElem, DeviceTags::CPU>
	{
	public:
		static_assert(std::is_same<RemConstRef<tElem>, tElem>::value, "tElem is not an available type");
		using ElementType = tElem;
		using DeviceType = DeviceTags::CPU;

	private:
		ContinuousMemory<ElementType, DeviceType> mem_;	//指向保存矩阵元素的数组开头位置
		size_t rowNum_;
		size_t colNum_;
		size_t rowLen_;

		//子矩阵构造函数
		Matrix(std::shared_ptr<ElementType> pMem, ElementType* pMemStart, size_t rowNum, size_t colNum, size_t rowLen) :mem_(pMem, pMemStart), rowNum_(rowNum), colNum_(colNum), rowLen_(rowLen)
		{}

	public:
		Matrix(size_t rowNum=0, size_t colNum=0):mem_(rowNum*colNum), rowNum_(rowNum), colNum_(colNum), rowLen_(colNum)
		{}

		bool operator==(const Matrix& m) const
		{
			return (mem_ == m.mem_) && (rowNum_ == m.rowNum_) && (colNum_ == m.colNum_) && (rowLen_ == m.rowLen_);
		}

		template <typename tOtherType>
		bool operator==(const tOtherType&) const
		{
			return false;
		}

		template <typename T>
		bool operator!=(const T& m) const
		{
			return !(operator==(m));
		}

		size_t RowNum() const
		{
			return rowNum_;
		}

		size_t ColNum() const
		{
			return colNum_;
		}

		//矩阵元素级读操作，返回矩阵元素的副本而不是指针或者引用
		const auto operator() (size_t sRowId, size_t sColId) const
		{
			assert((sRowId < rowNum_) && (sColId < colNum_));
			return (mem_.RawMemory())[sRowId * rowLen_ + sColId];	//mem_.RawMemory()返回raw pointer
		}

		//矩阵元素级写操作
		void SetValue(size_t sRowId, size_t sColId, ElementType eVal)
		{
			assert(mem_.UseCount() == 1);  //只有当前对象没有与其它对象共享内存时，才可以进行写操作
			assert((sRowId < rowNum_) && (sColId < colNum_));
			(mem_.RawMemory())[sRowId * rowLen_ + sColId] = eVal;
		}

		Matrix SubMatrix(size_t sRowB, size_t sRowE, size_t sColB, size_t sColE) const
		{
			assert((sRowB < rowNum_) && (sColB < colNum_));
			assert((sRowE < rowNum_) && (sColE < colNum_));

			auto pos = mem_.RawMemory() + sRowB * rowLen_ + sColB;
			return Matrix(mem_.SharedPtr(), pos, sRowE - sRowB+1, sColE - sColB+1, rowLen_);
		}

        auto EvalRegister() const
        {}

		friend struct LowerAccessImpl<Matrix<tElem, DeviceTags::CPU>>;
		friend class Batch<tElem, DeviceTags::CPU, CategoryTags::Matrix>;
	};

	template <typename tElem>
	struct LowerAccessImpl<Matrix<tElem, DeviceTags::CPU>>
	{
	private:
		Matrix<tElem, DeviceTags::CPU> m_;

	LowerAccessImpl(Matrix<tElem, DeviceTags::CPU> m):m_(std::move(m))
	{}

	auto MutableRawMemory()
	{
		return m_.mem_.RawMemory();
	}

	const auto RawMemory() const
	{
		return m_.mem_.RawMemory();
	}

	size_t RowLen() const
	{
		return m_.rowLen_;
	}
	};
}