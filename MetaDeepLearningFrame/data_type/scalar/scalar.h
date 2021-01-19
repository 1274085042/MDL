#pragma once

#include "../facilities/traits.h"

namespace MDL
{
	template <typename tElem, typename tDevice=DeviceTags::CPU>
	class Scalar
	{
	private:
		//static_assert(std::is_same<RemConstRef<tElem>, tElem>::value);
		//static_assert(std::is_same<RemConstRef<tElem>, tElem>::value);
		static_assert(std::is_same<RemConstRef<tElem>, tElem>::value, "tElem is not an available type");

	public:
		using ElementType = tElem;
		using DeviceType = tDevice;
	
	private:
		ElementType eElem_;

	public:
		//使用编译器提供的默认拷贝构造函数， 赋值运算符
		Scalar(ElementType elem=ElementType()):eElem_(elem)
		{}

		auto& Value()		//用于修改成员变量
		{
			return eElem_;
		}

		auto Value() const //返回eElem_的副本
		{
			return eElem_;
		}

		bool operator==(const Scalar& sVal) const
		{
			return eElem_ == sVal.eElem_;
		}

		template <typename tOtherType>
		bool operator==(const tOtherType&) const
		{
			return false;
		}

		template<typename tType>
		bool operator!=(const tType& tVal) const
		{
			return !(operator==(tVal));
		}

		//求值注册
		auto EvalRegister() const
		{

		}
	};

	template <typename tElem, typename tDevice>
	constexpr bool IsScalar<Scalar<tElem, tDevice>> = true;
}