#pragma once

#include "../scalar/scalar.h"
#include "../facilities/tags.h"
#include "../facilities/traits.h"
#include "../matrices/matrices.h"
#include <memory>
#include <vector>
#include <cstddef>
#include <stdexcept>

//CusVector跟Batch的区别：CusVector可以动态扩展内存，Batch是固定大小的
namespace MDL
{

	//虽然矩阵列表和标量列表都属于CusVector，但是它们提供的接口不一样，
	//因此要为矩阵列表和标量列表提供不同的实现，为达到该目的引入辅助类CusVectorImp
	template <typename tData, typename tDataCategory>
	class CusVectorImp;

	//CusVector模板参数只有一个，比如矩阵类型或者标量类型
	//CusVector根据类型提供的接口得到相应的计算单元和计算设备
	template <typename tData>
	class CusVector :public CusVectorImp<tData, DataCategory<tData>>
	{
	public:
		using ElementType = typename tData::ElementType;
		using DeviceType = typename tData::DeviceType;
		using CusVectorImp<tData, DataCategory<tData>>::CusVectorImp;    //使用基类中的构造函数
	};

	template <typename tData>
	class CusVectorImp<tData, CategoryTags::Matrix>
	{
	public:
		using ElementType = typename tData::ElementType;
		using DeviceType = typename tData::DeviceType;

	private:
		size_t rowNum_;
		size_t colNum_;
		std::shared_ptr<std::vector<tData>> buffer_;
		//EvalBuffer<Batch<ElementType, DeviceType, CategoryTags::Matrix>> evalBuf_;

	public:
		CusVectorImp(size_t rowNum, size_t colNum) :rowNum_(rowNum), colNum_(colNum), buffer_(new std::vector<tData>())
		{}

		//如果IsIterator<tIterator>的返回值为true，则enable_if_t<IsIterator<tIterator>>将推导为void
		template <typename tIterator, std::enable_if_t<IsIterator<tIterator>>* = nullptr>
		CusVectorImp(tIterator b, tIterator e) : rowNum_(0), colNum_(0), buffer_(new std::vector<tData>(b, e))
		{
			const auto& buffer = *buffer_;
			if (!buffer.empty())
			{
				rowNum_ = buffer[0].RowNum();
				colNum_ = buffer[0].ColNum();

				for (size_t i = 1; i < buffer.size(); i++)
				{
					if ((buffer[i].RowNum() != rowNum_) || (buffer[i].ColNum() != colNum_))
					{
						throw std::runtime_error("Dimension mismatch");
					}
				}
			}
		}

		bool AvailableForWrite() const
		{
			return buffer_.use_count() == 1;
		}

		size_t RowNum() const
		{
			return rowNum_;
		}

		size_t ColNum() const
		{
			return colNum_;
		}

		size_t BatchNum() const
		{
			return buffer_->size();
		}

		size_t Size() const
		{
			return buffer_->size();
		}

		void Push_Back(tData tMat)
		{
			assert(AvailableForWrite());
			if ((tMat.RowNum() != rowNum_) || (tMat.ColNum() != colNum_))
			{
				throw std::runtime_error("Dimension mismatch");
			}

			buffer_->emplace_back(std::move(tMat));
		}

		//什么时候用到Emplace_Back???
		template <typename ...tArgs> 
		void Emplace_Back(tArgs &&...args) //在模板函数中 && 代表通用引用符，因此函数体中尽可能用forward<>()进行转发提升性能
		{
			assert(AvailableForWrite());
			tData tmp(std::forward<tArgs>(args)...); //?tmp的构造函数
			if ((tmp.RowNum() != rowNum_) || (tmp.ColNum() != colNum_))
			{
				throw std::runtime_error("Dimension mismatch");
			}

			buffer_->emplace_back(std::move(tmp));
		}

		void Reserve(size_t num)
		{
			assert(AvailableForWrite());
			buffer_->reserve(num);
		}

		bool Empty()
		{
			return buffer_->empty();
		}

		const auto& operator[](size_t sId) const
		{
			return (*buffer_)[sId];
		}

		auto& operator[](size_t sId)
		{
			return (*buffer_)[sId];
		}

		auto Begin()
		{
			return buffer_->begin();
		}

		auto End()
		{
			return buffer_->end();
		}

		bool operator==(const CusVector<tData>& vMat) const
		{
			const CusVectorImp<tData, CategoryTags::Matrix>& tmp = static_cast<const CusVectorImp<tData, CategoryTags::Matrix>&>(vMat);
			return buffer_ == tmp.buffer_;
		}

		template <typename tOtherType>
		bool operator==(const tOtherType &) const
		{
			return false;
		}

		template <typename T>
		bool operator!=(const T& tVal) const
		{
			return !(operator==(tVal));
		}

		auto EvalRegister() const
		{

		}
        
        void Clear()
		{
			assert(AvailableForWrite());
			buffer_->clear();
		}
	};

	template <typename tData>
	class CusVectorImp<tData, CategoryTags::Scalar>
	{
	public:
		using ElementType = typename tData::ElementType;
		using DeviceType = typename tData::DeviceType;

	private:
		std::shared_ptr<std::vector<tData>> buffer_;
		//EvalBuffer<Batch<ElementType, DeviceType, CategoryTags::Scalar>> evalBuf_;

	public:
		CusVectorImp(size_t rowNum=0, size_t colNum=0):buffer_(new std::vector<tData> ())  //rowNum? colNum?
		{}
		template <typename tIterator, std::enable_if_t<IsIterator<tIterator>>* =nullptr>
		CusVectorImp(tIterator b, tIterator e):buffer_(new std::vector<tData>(b,e))
		{}

		bool AvailableForWrite() const
		{
			return buffer_.use_count() == 1;
		}

		size_t BatchNum() const
		{
			return buffer_->size();
		}

		size_t Size() const
		{
			return buffer_->size();
		}

		void Push_Back(tData tScalar)
		{
			assert(AvailableForWrite());
			buffer_->emplace_back(tScalar);
		}

		//什么时候用到Emplace_Back???
		template<typename ... tArgs>
		void Emplace_Back(tArgs&& ...args)
		{
			assert(AvailableForWrite());
			tData tmp(std::forward<tArgs>(args)...);
			buffer_->emplace_back(std::move(tmp));
		}

		void Reserve(size_t num)
		{
			assert(AvailableForWrite());
			buffer_->reserve(num);
		}

		bool Empty() const
		{
			return buffer_->empty();
		}

		const auto& operator[](size_t tId) const
		{
			return (*buffer_)[tId];
		}

		auto& operator[](size_t tId) 
		{
			return (*buffer_)[tId];
		}

		auto Begin()
		{
			return buffer_->begin();
		}

		auto End()
		{
			return buffer_->End();
		}

		bool operator==(const CusVector<tData>& vSca) const
		{
			//static_cast 将派生类的引用CusVector<tData>& 转换为基类的引用 CusVectorImp<tData, CategoryTags::Scalar>& ，该转换是安全的（向上转换）
			const CusVectorImp<tData, CategoryTags::Scalar>& tmp = static_cast<const CusVectorImp<tData, CategoryTags::Scalar>&> (vSca);
			return buffer_ == tmp.buffer_;
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

        void Clear()
		{
			assert(AvailableForWrite());
			buffer_->clear();
		}
	};

	//引入IsMatrixBach特化，建立类型与类别之间的关系
	template <typename tData>
	constexpr bool IsMatrixBatch<CusVector<tData>> = IsMatrix<tData>;

	//引入IsScalarBatch特化
	template <typename tData>
	constexpr bool IsScalarBatch<CusVector<tData>> = IsScalar<tData>;
	template <typename tData>
	constexpr bool IsScalarBatch<CusVector<tData> &> = IsScalar<tData>;
	template <typename tData>
	constexpr bool IsScalarBatch<CusVector<tData> &&> = IsScalar<tData>;
	template <typename tData>
	constexpr bool IsScalarBatch<const CusVector<tData>> = IsScalar<tData>;
	template <typename tData>
	constexpr bool IsScalarBatch<const CusVector<tData>&> = IsScalar<tData>;
	template <typename tData>
	constexpr bool IsScalarBatch<const CusVector<tData> &&> = IsScalar<tData>;

	//MakeCusVector用于简化使用CusVectorImp(tIterator b, tIterator e)构造对象
	template<typename tIterator>
	auto MakeCusVector(tIterator b, tIterator e)
	{
		using Type =typename std::iterator_traits<tIterator>::value_type;
		using RawType = RemConstRef<Type>;

		return CusVector<RawType>(b, e);
	}

}