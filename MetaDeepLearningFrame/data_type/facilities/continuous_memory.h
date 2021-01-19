#pragma once

#include "traits.h"
#include "allocators.h"

namespace MDL
{
	/*
	* ContinuousMemory模板类对Allocator分配的内存进行维护
	* tElem表示计算单元类型
	* tDevice表示设备类型
	*/
	template <typename tElem, typename tDevice>
	class ContinuousMemory
	{
	private:
		//static_assert(std::is_same<RemConstRef<tElem>, tElem>::value);
		static_assert(std::is_same<RemConstRef<tElem>, tElem>::value, "tElem is not an available type");
		using ElementType = tElem;
		std::shared_ptr<ElementType> pMem_;
		ElementType* pMemStart_;   //raw pointer,读取数据时，使用pMemStart_

	public:
		ContinuousMemory(size_t sSize) :pMem_(Allocator<tDevice>::template Allocate<ElementType>(sSize)), pMemStart_(pMem_.get())
		{}

		//构造子矩阵时使用
		ContinuousMemory(std::shared_ptr<ElementType> pMem, ElementType * pMemStart):pMem_(std::move(pMem)), pMemStart_(pMemStart)
		{}

		auto RawMemory() const
		{
			return pMemStart_;
		}

		const std::shared_ptr<ElementType> SharedPtr() const
		{
			return pMem_;
		}

		//判等操作为求值优化提供了相应的支持
		bool operator==(const ContinuousMemory& cm) const
		{
			return (pMem_ == cm.pMem_) && (pMemStart_ == cm.pMemStart_);
		}

		bool operator!=(const ContinuousMemory& cm) const
		{
			return !(operator==(cm));
		}

		//UseCount()为1时，表示当前进行元素级写操作的对象并不与其它对象共享内存，写操作可以进行
		//如果该对象与其它对象共享内存，那么写操作被禁止
		size_t UseCount() const
		{
			return pMem_.use_count();
		}

	};
}

