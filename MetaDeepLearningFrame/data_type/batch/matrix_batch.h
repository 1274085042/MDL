#pragma once

#include "batch.h"
#include "../matrices/cpu_matrix.h"

namespace MDL
{
	template <typename tElem, typename tDevice>
	struct LowerAccessImpl<Batch<tElem, tDevice, CategoryTags::Matrix>>;

	template <typename tElem, typename tDevice>
	class Batch<tElem, tDevice, CategoryTags::Matrix>
	{
	public:
		using ElementType = tElem;
		using DeviceType = tDevice;
		
	private:
		ContinuousMemory<ElementType, DeviceType> mem_;
		size_t rowNum_;
		size_t colNum_;
		size_t batchNum_;
		size_t rowLen_;
		size_t rawMatrixSize_;

		Batch(std::shared_ptr<ElementType> pMem, ElementType *pMemStart, size_t rowNum, size_t colNum, size_t batchNum, size_t rowLen, size_t rawMatrixSize):mem_(pMem, pMemStart), rowNum_(rowNum), colNum_(colNum), batchNum_(batchNum), rowLen_(rowLen), rawMatrixSize_(rawMatrixSize)
		{}

	public:
		Batch(size_t batchNum = 0, size_t rowNum=0, size_t colNum=0):mem_(batchNum*rowNum*colNum), rowNum_(rowNum), colNum_(colNum), batchNum_(batchNum), rowLen_(colNum), rawMatrixSize_(rowNum*colNum)
		{}
		
		bool operator==(const Batch& b) const  //Batch是本类型class Batch<tElem, tDevice, CategoryTags::Matrix> ？？？
		{
			return (mem_ == b.mem_) && (rowNum_ == b.rowNum_) && (colNum_ == b.colNum_) && (batchNum_ == b.batchNum_) && (rowLen_ == b.rowLen_) && (rawMatrixSize_ == b.rawMatrixSize_);
		}

		template <typename tOtherType>
		bool operator==(const tOtherType &) const
		{
			return false;
		}

		template <typename T>
		bool operator!=(const T& b) const
		{
			return !(operator==(b));
		}

		size_t RowNum() const
		{
			return rowNum_;
		}

		size_t ColNum() const
		{
			return colNum_;
		}

		size_t BatchNum() const
		{
			return batchNum_;
		}

		size_t AvailableForWrite() const
		{
			return mem_.UseCount();
		}

		//访问矩阵中的某个元素 
		//Batch<int, DeviceTags::CPU, CategoryTags::Matrix> bm;
		//auto x=bm[1](2,3)
		const auto operator[](size_t sBatchId) const
		{
			assert(sBatchId < batchNum_);
			auto pos = mem_.RawMemory() + sBatchId * rawMatrixSize_;

			return Matrix<ElementType, DeviceType>(mem_.SharedPtr(), pos, rowNum_, colNum_, rowLen_);
		}

		void SetValue(size_t sBatchId, size_t sRowId, size_t sColId, ElementType eVal)
		{
			assert(mem_.UseCount() == 1);
			assert((sBatchId < batchNum_) && (sRowId < rowNum_) && (sColId < colNum_));

			size_t pos = sBatchId *rawMatrixSize_+ sRowId *rowLen_+ sColId;
			(mem_.RawMemory())[pos] = eVal;
		}

		Batch SubMatrixBatch(size_t sRowB, size_t sRowE, size_t sColB, size_t sColE) const
		{
			assert((sRowB < rowNum_) && (sColB < colNum_));
			assert((sRowE < rowNum_) && (sColE < colNum_));

			auto pos = mem_.RawMemory() + sRowB * rowLen_ + sColB;

			return Batch(mem_.SharedPtr(), pos, sRowE - sRowB + 1, sColE - sColB + 1, batchNum_, rowLen_, rawMatrixSize_);
		}

		auto EvalRegister() const
		{}

		friend struct LowerAccessImpl<Batch<tElem, tDevice, CategoryTags::Matrix>>;
	};


	template <typename tElem, typename tDevice>
	struct LowerAccessImpl<Batch<tElem, tDevice, CategoryTags::Matrix>>
	{
	private:
		Batch<tElem, tDevice, CategoryTags::Matrix> b_;

	public:
		LowerAccessImpl(Batch<tElem, tDevice, CategoryTags::Matrix> b):b_(move(b))		//调用Batch默认的复制构造函数
		{}

		auto MutableRawMemory()
		{
			return b_.mem_.RawMemory();
		}

		const auto RawMemory() const
		{
			return b_.mem_.RawMemory();
		}

		size_t RowLen() const
		{
			return b_.rowLen_;
		}

		size_t RowMatrixSize() const
		{
			return b_.rawMatrixSize_;
		}
	};

}