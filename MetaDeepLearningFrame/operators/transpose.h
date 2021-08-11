#pragma once

#include "operators.h"
#include "./facilities/tags.h"
#include "../facilities/traits.h"
#include "./facilities/organizer.h"
#include "../data_type/facilities/traits.h"

namespace MDL
{
    /*
    * 转置运算（transpose）改变了矩阵的行和列，所以需要特化OperOrganier
    */
    template<>
    class OperOrganizer<UnaryOpTags::Transpose, CategoryTags::Matrix>
    {
    private:
        size_t rowNum_;
        size_t colNum_;

    public:
        template <typename tData>
        OperOrganizer(const tData & data):rowNum_(data.ColNum()), colNum_(data.RowNum())
        {
        }

        size_t RowNum() const
        {
            return rowNum_;
        }

        size_t ColNum() const
        {
            return colNum_;
        }
    };

    template<>
    class OperOrganizer<UnaryOpTags::Transpose, CategoryTags::MatrixBatch>:public OperOrganizer<UnaryOpTags::Transpose, CategoryTags::Matrix>
    {
    private:
        size_t batchNum_;

    public:
        using BaseType = OperOrganizer<UnaryOpTags::Transpose, CategoryTags::Matrix>;

        template <typename tData>
        OperOrganizer(const tData & data):BaseType(data), batchNum_(data.BatchNum())
        {
        }   

        size_t BatchNum() const
        {
            return batchNum_;
        }
    };

    template <typename T>
    struct OperTranspose_
    {
    private:
        using RawType = typename RemConstRef<T>;
    
    public:
        static constexpr bool vaild = IsMatrix<RawType> || IsMatrixBatch<RawType>;
        
        static auto Eval(T && t)
        {
            using ResType = UnaryOp<UnaryOpTags::Transpose, RawType>;
            return ResType(std::forward<T>(t));
        }    
    };

    template <typename T, std::enable_if_t<OperTranspose_<T>::vaild> * = nullptr>
    auto TransPose(T && t)
    {
        return OperTranspose_<T>::Eval(std::forward<T>(t));
    }
}