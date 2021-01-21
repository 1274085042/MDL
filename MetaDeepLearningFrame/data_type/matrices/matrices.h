#pragma once

#include "../facilities/traits.h"

namespace MDL
{
	//template <typename  tElem, typename tDevice>
	//class Matrix;

	template <typename tElem, typename tDevice>
	constexpr bool IsMatrix<Matrix<tElem, tDevice>> = true;
}