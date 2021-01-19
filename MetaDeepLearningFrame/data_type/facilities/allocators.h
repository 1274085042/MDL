#pragma once

#include "tags.h"
#include <deque>
#include <mutex>
#include <memory>
#include <unordered_map>

namespace MDL
{
	/*
	* Allocator存储空间的分配和释放
	*/
	template <typename tDevice>
	struct Allocator;

	template <>
	struct Allocator<DeviceTags::CPU>
	{
	private:
		static std::mutex& GetMutex()
		{
			static std::mutex mInst;
			return mInst;
		}

		struct  AllocateHelper
		{
			std::unordered_map<size_t, std::deque<void*>> uMemBuffer;
			
			~AllocateHelper()
			{
				for (auto& p : uMemBuffer)
				{
					auto& aVec = p.second;
					for (auto& p1 : aVec)
					{
						char* buf = (char*)(p1);
						delete[] buf;
					}
					aVec.clear();
				}
			}
		};

		struct DesImpl
		{
		private:
			std::deque<void*>& dRefPool_;

		public:
			DesImpl(std::deque<void*>& dRefPool) :dRefPool_(dRefPool)
			{}

			void operator()(void* pVal) const
			{
				std::lock_guard<std::mutex> lg(GetMutex());   //加锁
				dRefPool_.push_back(pVal);
			}

		};

	public:
		template <typename T>
		static std::shared_ptr<T> Allocate(size_t sElemSize)  
		{
			if (sElemSize == 0)
			{
				return nullptr;
			}

			sElemSize = (sElemSize * sizeof(T) + 1023) & (std::numeric_limits<size_t>::max() ^ 1023);	//sElemSize是1024的倍数

			std::lock_guard<std::mutex> lg(GetMutex());

			static AllocateHelper aHelper;
			auto& slot = aHelper.uMemBuffer[sElemSize];		//uMemBuffer:unordered_map<size_t, deque<void *>>	slot:deque<void *>

			if (slot.empty())
			{
				auto raw_buf = (T*) new char[sElemSize];
				return std::shared_ptr<T>(raw_buf, DesImpl(slot));
			}
			else
			{
				void* mem = slot.back();
				slot.pop_back();
				return std::shared_ptr<T>((T*)mem, DesImpl(slot));
			}
		}
	};
}