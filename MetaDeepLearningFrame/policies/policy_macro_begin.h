#pragma once

/*
* 采用虚继承是防止在多继承的情形中，新定义的类型和常量不具有支配地位
*/
//修改AccuTypeCate
#define TypePolicyObj(PolicyName, Ma, Mi, Typ) \
struct PolicyName:virtual public Ma \
{ \
	using MinorClass=Ma::Mi##TypeCate; \
	using Mi=Ma::Mi##TypeCate::Typ; \
}

//修改IsAveValueCate
#define ValuePolicyObj(PolicyName, Ma, Mi, Val) \
struct PolicyName:virtual public Ma \
{ \
	private: \
		using type1=decltype(Ma::Mi); \
		using type2=RemConstRef<type1>; \
	public:\
		using MinorClass=Ma::Mi##ValueCate; \
		static constexpr type2 Mi=static_cast<type2>(Val); \
}

//修改ValueTypeCate
#define TypePolicyTemplate(PolicyName, Ma, Mi) \
template <typename T> \
struct PolicyName: virtual public Ma \
{ \
	using MinorClass=Ma::Mi##TypeCate; \
	using Mi=T; \
}

//修改IsAveValueCate
#define ValuePolicyTemplate(PolicyName, Ma, Mi) \
template <RemConstRef<decltype(Ma::Mi)> V> \
struct PolicyName:virtual public Ma \
{ \
	private: \
		using type1=decltype(Ma::Mi); \
		using type2=RemConstRef<type1>; \
	public:\
		using MinorClass=Ma::Mi##ValueCate; \
		static constexpr type2 Mi=V; \
};