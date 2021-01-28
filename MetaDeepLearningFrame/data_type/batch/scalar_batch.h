#pragma once
#include "batch.h"
#include "../facilities/tags.h"
#include "../facilities/lower_access.h"
#include "../facilities/continuous_memory.h"

#include <cassert>
namespace MDL
{
	template <typename tElem, typename tDevice>
	struct LowerAccessImpl<Batch<tElem, tDevice, CategoryTags::Scalar>>;

	template <typename tElem>
	class Batch<tElem, DeviceTags::CPU, CategoryTags::Scalar>
	{
	public:
		using ElementType = tElem;
		using DeviceType = DeviceTags::CPU;

	private:
		ContinuousMemory<ElementType, DeviceType> mem_;
		size_t len_;

	public:
		Batch(size_t length = 0) :mem_(length), len_(length)
		{}

		bool AvailableForWrite() const
		{
			return mem_.UseCount() == 1;
		}

		size_t BatchNum() const
		{
			return len_;
		}

		void SetValue(size_t sId,ElementType val)
		{
			assert(AvailableForWrite());
			assert(sId < len_);
			(mem_.RawMemory())[sId] = val;
		}

		const auto operator[](size_t sId) const
		{
			assert(sId < len_);
			return (mem_.RawMemory())[sId];
		}

		bool operator==(const Batch &b) const
		{
			return (mem_ == b.mem_) && (len_ == b.len_);
		}

		template <typename tOtherType>
		bool operator==(const tOtherType&) const
		{
			return false;
		}

		template <typename T>
		bool operator!=(const T& t)const
		{
			return !(operator==(t));
		}

		auto EvalRegister() const
		{

		}

		friend LowerAccessImpl<Batch<ElementType, DeviceType, CategoryTags::Scalar>>;
	};

	template <typename tElem, typename tDevice>
	struct LowerAccessImpl<Batch<tElem, tDevice, CategoryTags::Scalar>>
	{
	private:
		Batch<tElem, tDevice, CategoryTags::Scalar> b_;

	public:
		LowerAccessImpl(Batch<tElem, tDevice, CategoryTags::Scalar> b):b_(std::move(b))
		{}

		auto MutableRawMemory()
		{
			return b_.mem_.RawMemory();
		}

		const auto RawMemory() const
		{
			return b_.mem_.RawMemory();
		}
	};
}