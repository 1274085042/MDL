#pragma once
#include "./facilities/tags.h"
#include "./facilities/traits.h"
#include "./batch/matrix_batch.h"
#include "../facilities/traits.h"
#include "./matrices/cpu_matrix.h"
#include <memory>
#include <exception>

namespace MDL
{
    template <typename tElem, typename tDevice, typename tCategory>
    class DynamicCategory;

    template <typename tElem, typename tDevice>
    class DynamicCategory<tElem, tDevice, CategoryTags::Matrix>
    {
    private:
        size_t rowNum_;
        size_t colNum_;
    
    public:
        template<typename tBaseData>
        DynamicCategory(const tBaseData &bd):rowNum_(bd.RowNum()), colNum_(bd.ColNum())
        {}

        virtual bool operator==(const DynamicCategory &) =0;
        virtual bool operator!=(const DynamicCategory &) =0;

        size_t RowNum()
        {
            return rowNum_;
        }

        size_t ColNum()
        {
            return colNum_;
        }

        virtual ~DynamicCategory() = default;
    };

    template <typename tElem, typename tDevice>
    class DynamicCategory<tElem, tDevice, CategoryTags::MatrixBatch>
    {
    private:
        size_t rowNum_;
        size_t colNum_;
        size_t batchNum_;
    
    public:
        template<typename tBaseData>
        DynamicCategory(const tBaseData &bd):rowNum_(bd.RowNum()), colNum_(bd.ColNum()), batchNum_(bd.BatchNum())
        {}

        virtual bool operator==(const DynamicCategory &) =0;
        virtual bool operator!=(const DynamicCategory &) =0;

        size_t RowNum()
        {
            return rowNum_;
        }

        size_t ColNum()
        {
            return colNum_;
        }

        size_t BatchNum()
        {
            return batchNum_;
        }

        virtual ~DynamicCategory() = default; 
    };

    template <typename tBaseData>
    class DynamicWrapper:public DynamicCategory<typename tBaseData::ElementType, typename tBaseData::DeviceType, DataCategory<tBaseData>>
    {
    private:
        using DynamicCategoryType = DynamicCategory<typename tBaseData::ElementType, typename tBaseData::DeviceType, DataCategory<tBaseData>>;
        tBaseData baseData_;
    
    public:
        DynamicWrapper(tBaseData bd):DynamicCategoryType(bd), baseData_(move(bd))
        {}
        
        const tBaseData& BaseData()
        {
            return baseData_;
        }

        bool operator==(const DynamicCategory & dc)
        {
            try
            {
                const DynamicWrapper & dw = dynamic_cast<const DynamicWrapper &> (dc);
                return baseData_ == dw.baseData_;
            }
            catch(const std::bad_cast &)
            {
               return false;
            }         
        }

        bool operator!=(const DynamicCategory & dc)
        {
            return !(operator==(dc));
        }
    };

    /*
    * DynamicData是对具体的数据类别进一步的封装，从而屏蔽了不同的数据类之间的差异
    */
    template <typename tElem, typename tDevice, typename tCategory>
    class DynamicData;

    template <typename tElem, typename tDevice>
    class DynamicData<tElem, tDevice, CategoryTags::Matrix>
    {
    private:
        using DynamicCategoryType = DynamicCategory<tElem, tDevice, CategoryTags::Matrix>;
        std::shared_ptr<DynamicCategoryType> sp_;
    
    public:
        template<typename tBaseData>
        DynamicData(std::shared_ptr<DynamicWrapper<tBaseData>> sp):sp_(sp)
        {
        }

        template<typename T>
        const T* Data()
        {
            DynamicCategoryType *dt = sp_.get();
            DynamicWrapper<T> *dw = dynamic_cast<DynamicWrapper<T> *>(dt);

            return (dw? &(dw->BaseData()) : nullptr);
        }

        size_t RowNum()
        {
            return sp_->RowNum();
        }

        size_t ColNum()
        {
            return sp_->ColNum();
        }

        bool operator==(const DynamicData &dd)
        {
            if ((!sp_) && (!dd.sp_))
            {
                return true;
            }
            if((!sp_) || (!dd.sp_))
            {
                return false
            }

            const DynamicCategoryType &dt1 = *(sp_);
            const DynamicCategoryType &dt2 = *(dd.sp_);

            return dt1 == dt2;
        }

        template <typename tOtherType>
        bool operator==(const tOtherType & ot)
        {
            return false;
        }
        
        template <typename tOtherType>
        bool operator!=(const tOtherType & ot)
        {
            return !(operator==(ot));
        }

        bool IsEmpty()
        {
            return sp_==nullptr;
        }

    };
    
    template <typename tElem, typename tDevice>
    class DynamicData<typename tElem, typename tDevice, CategoryTags::MatrixBatch>
    {
    private:
        using DynamicCategoryType = DynamicCategory<tElem, tDevice, CategoryTags::MatrixBatch>;
        std::shared_ptr<DynamicCategoryType> sp_;
    
    public:
        template<typename tBaseData>
        DynamicData(std::shared_ptr<DynamicWrapper<tBaseData>> sp):sp_(sp)
        {
        }

        template<typename T>
        const T* Data()
        {
            DynamicCategoryType *dt = sp_.get();
            DynamicWrapper<T> *dw = dynamic_cast<DynamicWrapper<T> *>(dt);

            return (dw? &(dw->BaseData()) : nullptr);
        }

        size_t RowNum()
        {
            return sp_->RowNum();
        }

        size_t ColNum()
        {
            return sp_->ColNum();
        }

        size_t BatchNum()
        {
            return sp_->BatchNum();
        }

        bool operator==(const DynamicData &dd)
        {
            if ((!sp_) && (!dd.sp_))
            {
                return true;
            }
            if((!sp_) || (!dd.sp_))
            {
                return false
            }

            const DynamicCategoryType &dt1 = *(sp_.get());
            const DynamicCategoryType &dt2 = *(dd.sp_.get());

            return dt1 == dt2;
        }

        template <typename tOtherType>
        bool operator==(const tOtherType & ot)
        {
            return false;
        }
        
        template <typename tOtherType>
        bool operator!=(const tOtherType & ot)
        {
            return !(operator==(ot));
        }

        bool IsEmpty()
        {
            return sp_ == nullptr;
        }
    };

    template<typename T>
    auto MakeDynamicData(T&&t)
    {
        if constexpr(IsDynamic<T>)
        {
            return forward<T>(t);
        }
        else
        {
            using RawType = RemConstRef<T>;
            std::shared_ptr<DynamicCategory> sp = std::make_shared<DynamicWrapper<RawType>>(forward<T>(t));
            return DynamicData<typename RawType::ElementType, typename RawType::DeviceType, typename DataCategory<RawType>> (sp);
        }
    }

    template<typename T>
    constexpr bool IsDynamic = false;

    template<typename tElem, typename tDevice, typename tCate>
    constexpr bool IsDynamic<DynamicData<tElem, tDevice, tCate>> = true;

    template<typename tElem, typename tDevice, typename tCate>
    constexpr bool IsDynamic<DynamicData<tElem, tDevice, tCate> &> = true;

    template<typename tElem, typename tDevice, typename tCate>
    constexpr bool IsDynamic<DynamicData<tElem, tDevice, tCate> &&> = true;

    template<typename tElem, typename tDevice, typename tCate>
    constexpr bool IsDynamic<const DynamicData<tElem, tDevice, tCate> &> = true;

    template<typename tElem, typename tDevice, typename tCate>
    constexpr bool IsDynamic<const DynamicData<tElem, tDevice, tCate> &&> = true;

    template<typename tElem, typename tDevice>
    constexpr bool IsMatrix<DynamicData<tElem, tDevice, CategoryTags::Matrix>> =true;
    
    template<typename tElem, typename tDevice>
    constexpr bool IsMatrixBatch<DynamicData<tElem, tDevice, CategoryTags::MatrixBatch>> =true;
}
