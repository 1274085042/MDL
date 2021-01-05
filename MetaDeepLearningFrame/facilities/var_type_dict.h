#pragma once

#include <memory>

namespace MNN
{
	namespace NSVarTypeDict
	{
		//供VarTypeDict使用的通用逻辑 
		struct NullParameter;

		//为Values添加占位类型
		template <size_t n, template <typename ...> class tContainer, typename ...T>
		struct Create_
		{
			using type =typename Create_<n - 1, tContainer, NullParameter, T...>::type;
		};
		template <template <typename ...> class tContainer, typename ...T>
		struct Create_<0, tContainer, T...>
		{
			//using type =typename tContainer<T...>;
			using type = tContainer<T...>;
		};

		//在异类词典中根据键值找到相应位置
		template <typename tKey_, size_t n, typename tCurKey_, typename ...T>
		struct Key2Pos_
		{
			constexpr static size_t pos= Key2Pos_<tKey_, n+1, T...>::pos;
		};
		template <typename tKey_, size_t n, typename ...T>
		struct Key2Pos_ <tKey_, n, tKey_, T...>
		{
			constexpr static size_t pos = n;
		};
		template <typename tKey_, typename ...T>
		constexpr size_t Key2Pos = Key2Pos_<tKey_, 0, T...>::pos;

		//如果使用Values<>::set<>()就会设置新的类型，因此使用NewTupleType来创建新的Values<>
		template<typename tType,size_t n, size_t s, typename tContainer, typename ...tRemainTypes>
		struct NewTupleType_;
		template<typename tType, size_t n, size_t s, template<typename...> class tContainer, typename ...tScanTypes, typename tCurType, typename ...tRemainTypes>
		struct NewTupleType_<tType, n, s, tContainer<tScanTypes...>, tCurType, tRemainTypes...>
		{
			using type =typename NewTupleType_<tType, n, s+1, tContainer<tScanTypes..., tCurType>, tRemainTypes...>::type;
		};
		template<typename tType, size_t n, template<typename...> class tContainer, typename ...tScanTypes, typename tCurType, typename ...tRemainTypes>
		struct NewTupleType_<tType, n, n, tContainer<tScanTypes...>, tCurType, tRemainTypes...>
		{
			//using type =typename tContainer<tScanTypes..., tType, tRemainTypes...>;
			using type = tContainer<tScanTypes..., tType, tRemainTypes...>;
		};
		template<typename tType, size_t n, typename tContainer, typename ...tRemainTypes>
		using NewTupleType =typename NewTupleType_<tType, n, 0, tContainer, tRemainTypes...>::type;

		//根据位置判断类型
		template <size_t pos, size_t s, typename tCurtype, typename ...tValueTypes>
		struct Pos2Type_
		{
			using type =typename Pos2Type_<pos, s+1, tValueTypes...>::type;
		};
		template <size_t pos, typename tCurtype, typename ...tValueTypes>
		struct Pos2Type_<pos, pos, tCurtype, tValueTypes...>
		{
			//using type = typename tCurtype;
			using type =  tCurtype;
		};
		template <size_t pos, typename ...tValueTypes>
		using Pos2Type = typename Pos2Type_<pos, 0, tValueTypes...>::type;
	}

	template <typename ...tKey>
	struct VarTypeDict
	{

		template <typename ...tValueTypes>
		struct Values
		{
		private:
			//数据存储区域是一个智能指针数组，数组中的每个元素类型是void
			//因为void可以与任意指针类型相互转换
			std::shared_ptr<void> sTuple_[sizeof...(tValueTypes)];

		public:
			//如果自定义了构造函数，系统提供的默认无参构造函数就会被屏蔽，
			//使用default，是为了恢复系统提供的无参构造函数
			Values() = default;

			Values(std::shared_ptr<void>(&&input)[sizeof...(tValueTypes)])
			{
				for (size_t i = 0; i < sizeof...(tValueTypes); i++)
				{
					sTuple_[i] = move(input[i]);  //important
				}
			}

			template <typename tKey_, typename tType_>
			auto  Set(tType_&& val) &&		//Set()函数后的&&，表示该函数只能被右值使用
			{								//根据传入的实参val将自动推导出tType_
				using namespace NSVarTypeDict;
				constexpr static size_t pos = Key2Pos<tKey_, tKey...>;

				using rawType = std::decay_t<tType_>;

				rawType* tmp = new rawType(std::forward<tType_>(val));

				sTuple_[pos] = std::shared_ptr<void>(tmp,
					[](void* ptr)
					{
						rawType* p = static_cast<rawType*>(ptr);
						delete p;
					});

				using newValues = NewTupleType<rawType, pos, Values<>, tValueTypes...>;  //创建新的Values<>类型

				return newValues(std::move(sTuple_));									//使用新的类型创建对象并返回
			}

			template <typename tKey_>
			auto& Get() const
			{
				using namespace NSVarTypeDict;
				constexpr static size_t pos = Key2Pos<tKey_, tKey...>;
				using type = Pos2Type<pos, tValueTypes...>;

				void* tmp = sTuple_[pos].get();
				type* res = static_cast<type*>(tmp);
				return *res;
			}
		};

		static auto Create()			//auto 可以用作返回类型？？？
		//对VarTypeDict进行特化产生VarTypeDict<tKey...>::Values<tValueType...>
		//返回类型VarTypeDict<tKey...>::Values<tValueType...>的特化对象
		{
			using namespace NSVarTypeDict;
			using type = typename Create_<sizeof...(tKey), Values>::type;
			return type{};
		}
	};
}