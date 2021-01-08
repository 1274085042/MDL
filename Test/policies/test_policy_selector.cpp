#include "traits.h"
#include "policy_selector.h"
#include "policy_container.h"
#include <cmath>
#include <assert.h>
#include <iostream>

using namespace MDL;
/*
* 将policy类型按照功能划分成不同的组，每个模板可以使用一个或多个policy组中的类型
* 为了描述policy类型所属的组以及类型之间的互斥性，引入两个属性major class（主属性）表示类型所属的组；而minor class（次要属性）描述了互斥信息
* 如果两个policy类型的major class和minor class均相同，那么两者是互斥的，不能被同时使用
* 
* 命名方式：类型policy，键名加上TypeCate后缀；数值policy，键名加上ValueCate后缀
* 以下代码说明：
* 组名为AccPolicy,组内包含3个policy
* AccTypeCate（minor class），累积policy
* IsAveValueCate（minor class），是否求均值policy
* ValueTypeCate（minor class），返回值类型policy
*/
struct AccPolicy
{
	using MajorClass = AccPolicy;

	struct  AccuTypeCate
	{
		struct Add;		//以连加的方式累积
		struct Mul;		//以连乘的方式累积
	};
	using Accu = AccuTypeCate::Add;	//键为Accu，默认值为Add

	struct IsAveValueCate;
	static constexpr bool IsAve = false; //键为IsAve（数值Policy），默认值为false

	struct ValueTypeCate;
	using Value = float;
};

#include "policy_macro_begin.h"
TypePolicyObj(pAddAccu, AccPolicy, Accu, Add);
TypePolicyObj(pMulAccu, AccPolicy, Accu, Mul);
ValuePolicyObj(pAve, AccPolicy, IsAve, true);
ValuePolicyObj(pNoAve, AccPolicy, IsAve, false);
TypePolicyTemplate(pValueTypeIs, AccPolicy, Value);
ValuePolicyTemplate(pAvePolicyIs, AccPolicy, IsAve);
#include "policy_macro_end.h"

template <typename ...tPolicies>
struct Accumulator
{
	using TPolicyCont = PolicyContainer<tPolicies...>;
	using TPolicyRes = PolicySelect<AccPolicy, TPolicyCont>;

	using AccuType = typename TPolicyRes::Accu;
	static constexpr bool bIs_Ave = TPolicyRes::IsAve;
	using ValueType = typename TPolicyRes::Value;

	template <typename T>
	static auto Eval(const T& t)  //C++14开始可以使用auto作为函数返回类型
	{
		if constexpr (std::is_same<AccuType, AccPolicy::AccuTypeCate::Add>::value)  //if constexpr 在C++17引入
		{
			ValueType count = 0;
			ValueType res = 0;
			for (const auto& x : t)
			{
				res += x;
				count += 1;
			}

			if constexpr (bIs_Ave)
			{
				return res / count;
			}
			else
			{
				return res;
			}
		}
		else if constexpr (std::is_same<AccuType, AccPolicy::AccuTypeCate::Mul>::value)
		{
			ValueType count = 0;
			ValueType res = 1;
			for (const auto& x : t)
			{
				res *= x;
				count += 1;
			}

			if constexpr (bIs_Ave)
			{
				return pow(res,1.0/count);
			}
			else
			{
				return res;
			}
		}
		else
		{
			static_assert(DependencyFalse<AccuType>);
		}
	}
};

int main()
{
	using std::cout;
	using std::endl;
	cout << "Test policy selector..."<<endl;
	const int a[] = { 1,2,3,4,5 };
	//auto res1=Accumulator<>::Eval(a);
	//assert(fabs(Accumulator<>::Eval(a) - 15) < 0.0001);
	auto res1 = Accumulator<>::Eval(a) - 15;
	auto res2 = Accumulator<pMulAccu>::Eval(a);
	auto res3 = Accumulator<pMulAccu,pAve>::Eval(a);
	auto res4 = Accumulator<pAve, pMulAccu>::Eval(a);
	auto res5 = Accumulator<pAvePolicyIs<true>, pMulAccu>::Eval(a);

	cout << res1 << endl;
	cout << res2 << endl;
	cout << res3 << endl;
	cout << res4 << endl;
	cout << res5 << endl;
	cout << "done" << endl;
}