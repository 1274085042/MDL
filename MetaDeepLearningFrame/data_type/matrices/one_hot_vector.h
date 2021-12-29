#pragma once

#include "./cpu_matrix.h"
#include "../facilities/traits.h"
#include "../facilities/lower_access.h"
#include "../../evaluate/facilities/eval_unit.h"
#include "../../evaluate/facilities/eval_plan.h"
#include "../../evaluate/facilities/eval_group.h"
#include "../../evaluate/facilities/eval_buffer.h"
//#include "../../evaluate/facilities/eval_handle.h"

#include <cassert>
//#include <memory>
#include <cstring>

namespace MDL
{
	template <typename tElem, typename tDevice>
	class EvalUnit;

	template<typename tElement>
	class EvalUnit<tElement, DeviceTags::CPU>:public BaseEvalUnit<DeviceTags::CPU>
	{
	private:
		EvalHandle<Matrix<tElement, DeviceTags::CPU>> resHandle_;
		size_t rowNum_;
		size_t colNum_;
		size_t val_;
	
	public:
		EvalUnit(EvalHandle<Matrix<tElement, DeviceTags::CPU>> resBuf, size_t rowNum, size_t colNum, size_t val):resHandle_(std::move(resBuf)), rowNum_(rowNum), colNum_(colNum), val_(val)
		{
			assert(val_ < (rowNum_ * colNum_));
		}

		void Eval()
		{
			auto &mutableData = resHandle_.MutableData();
			resHandle_.Allocate(rowNum_, colNum_);  //创建一个Matrix
			auto low = LowerAccess(mutableData);
			auto mem = low.MutableRawMemory();      //返回指向Matrix内存的指针
			memset(mem, 0, sizeof(tElement)*rowNum_*colNum_);  //数组中的所有元素初始化为0
			mem[val_] = 1;
			resHandle_.SetEval();
		}
	};

	template<typename tElem, typename tDevice>
	class OneHotVector
	{
	public:
		using ElementType = tElem;
		using DeviceType = tDevice; 

	private:
		size_t colNum_;
		size_t hotPos_;
		EvalBuffer<Matrix<ElementType, DeviceType>> evalBuf_;

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

		auto EvalRegister() 
		{
			using EvalUnitType = EvalUnit<ElementType, DeviceType>;
			using EvalGroupType = TrivialEvalGroup<EvalUnitType>;
			if(!evalBuf_.IsEvaluated())
			{
				auto evalHandle = evalBuf_.Handle();
				auto outputPtr = evalHandle.DataPtr();
				EvalUnitType unit(std::move(evalHandle), 1, colNum_, hotPos_);
				EvalPlan<DeviceType>::template Register<EvalGroupType>(std::move(unit), outputPtr, {});
			}

			return evalBuf_.ConstHandle();	
		}
	};

	template<typename tElem, typename tDevice>
	constexpr bool IsMatrix<OneHotVector<tElem, tDevice>> = true;
}