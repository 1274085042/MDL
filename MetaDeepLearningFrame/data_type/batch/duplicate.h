#pragma once

#include "../scalar/scalar.h"
#include "../facilities/traits.h"
#include "../matrices/matrices.h"

namespace MDL
{
	template <typename tData, typename tDataCategory>
	class DuplicateImp;

	//Duplicate对已有的矩阵和标量进行复制，因此不需要提供修改函数
	template <typename tData>
	class Duplicate :public DuplicateImp<tData, DataCategory<tData>>
	{
	public:
		using ElementType = typename tData::ElementType;
		using DeviceType = typename tData::DeviceType;
		using DuplicateImp<tData, DataCategory<tData>>::DuplicateImp;
	};

	template <typename tData>
	class DuplicateImp<tData, CategoryTags::Matrix>
	{
		static_assert(std::is_same<RemConstRef<tData>, tData>::value, "Unvailable data type");

	public:
		using ElementType = typename tData::ElementType;
		using DeviceType = typename tData::DeviceType;

	private:
		tData data_;
		size_t batchNum_;
		//EvalBuffer<Batch<ElementType, DeviceType, CategoryTags::Matrix>> evalBuf_;

	public:
		DuplicateImp(tData data, size_t batchNum) :data_(std::move(data)), batchNum_(batchNum)
		{}

		size_t RowNum() const
		{
			return data_.RowNum();
		}

		size_t ColNum() const
		{
			return data_.ColNum();
		}

		size_t BatchNum() const
		{
			return batchNum_;
		}

		const tData& Element() const
		{
			return data_;
		}
		
		bool operator==(const Duplicate<tData>& dMat) const
		{
			const DuplicateImp<tData, CategoryTags::Matrix>& tmp = static_cast<const DuplicateImp<tData, CategoryTags::Matrix>& >(dMat);
			return (data_ == tmp.data_) && (batchNum_ == tmp.batchNum_);
		}

		template <typename tOtherType>
		bool operator==(const tOtherType&) const
		{
			return false;
		}

		template <typename T>
		bool operator!=(const T& tMat) const
		{
			return !(operator==(tMat));
		}

		auto EvalRegister() const
		{

		}
	};

	template <typename tData>
	class DuplicateImp<tData, CategoryTags::Scalar>
	{
		static_assert(std::is_same<RemConstRef<tData>, tData>::value, "Unvailable data type");
	public:
		using ElementType = typename tData::ElementType;
		using DeviceType = typename tData::DeviceType;

	private:
		tData data_;
		size_t batchNum_;
		//EvalBuffer < Batch < ElementType, DeviceType, CategoryTags::Scalar>> evalBuf_;
	public:
		DuplicateImp(tData data, size_t batchNum) :data_(std::move(data)), batchNum_(batchNum)
		{}

		size_t BatchNum() const
		{
			return batchNum_;
		}

		const tData& Element() const
		{
			return data_;
		}

		bool operator==(const Duplicate<tData> & dSca) const
		{
			const DuplicateImp<tData, CategoryTags::Scalar>& tmp = static_cast<DuplicateImp<tData, CategoryTags::Scalar>&>(dSca);
			return  (data_ == tmp.data_) && (batchNum_ == tmp.batchNum_);
		}

		template <typename tOtherType>
		bool operator==(const tOtherType&) const
		{
			return false;
		}

		template <typename T>
		bool operator!=(const T& tSca) const
		{
			return !(operator==(tSca));
		}

		auto EvalRegister() const
		{

		}
	};

	template <typename tData>
	auto MakeDuplicate(size_t batchNum, tData &&data)
	{
		using RawType = RemConstRef<tData>;
		return Duplicate<RawType>(std::forward<tData>(data), batchNum);
	}

	template <typename tData, typename ...tArgs>
	auto MakeDuplicate(size_t batchNum, tArgs&& ...args)
	{
		using RawType = RemConstRef<tData>;
		RawType tmp = RawType(std::forward<tArgs>(args)...);
		return Duplicate<RawType>(std::move(tmp), batchNum);
	}
	
	template<typename tData>
	constexpr bool IsMatrixBatch<Duplicate<tData>> = IsMatrix<tData>;

	template<typename tData>
	constexpr bool IsScalarBatch<Duplicate<tData>> = IsScalar<tData>;
	template<typename tData>
	constexpr bool IsScalarBatch<Duplicate<tData> &> = IsScalar<tData>;
	template<typename tData>
	constexpr bool IsScalarBatch<Duplicate<tData> &&> = IsScalar<tData>;
	template<typename tData>
	constexpr bool IsScalarBatch<const Duplicate<tData>> = IsScalar<tData>;
	template<typename tData>
	constexpr bool IsScalarBatch<const Duplicate<tData> &> = IsScalar<tData>;
	template<typename tData>
	constexpr bool IsScalarBatch<const Duplicate<tData> &&> = IsScalar<tData>;
}