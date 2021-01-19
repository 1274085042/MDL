#pragma once

#include "traits.h"
#include <utility>

namespace MDL
{
	//底层数据访问
	template <typename tDataType>
	struct LowerAccessImpl;

	template <typename tDataType>
	auto LowerAccess(tDataType&& p)
	{ 
		using RawType = RemConstRef<tDataType>;
		return LowerAccessImpl<RawType>(std::forward<tDataType>(p));
	}
}
