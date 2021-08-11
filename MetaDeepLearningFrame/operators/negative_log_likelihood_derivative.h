#pragma once

#include "operators.h"
#include "./facilities/tags.h"
#include "./facilities/traits.h"
#include "../facilities/traits.h"
#include "../data_type/facilities/traits.h"
#include <type_traits>

namespace MDL
{
    /*
    * OperCategory_为什么需要特化？
    * 因为在facilities/category_cal.h中OperCategory_的逻辑是：
    * 判断所有输入类型的类别是否一致，一致的话返回第一个类型的类别
    * 而NegativeLogLikelihoodDerivative的参数类型是：
    * Scalar Matrix Matrix 或
    * ScalarBatch MatrixBatch MatrixBatch
    */
    template <>
    struct OperCategory_<TernaryOpTags::NegativeLogLikelihoodDerivative, CategoryTags::Scalar, CategoryTags::Matrix, CategoryTags::Matrix>
    {
        using type = CategoryTags::Matrix;
    };
    template <>
    struct OperCategory_<TernaryOpTags::NegativeLogLikelihoodDerivative, CategoryTags::ScalarBatch, CategoryTags::MatrixBatch, CategoryTags::MatrixBatch>
    {
        using type = CategoryTags::MatrixBatch;
    };

    /*
    * OperOrganizer为什么需要特化？
    * 因为在facilities/OperOrganizer.h中OperOrganizer逻辑是:
    * 需判断所有输入参数的维度是否一致
    * 而NegativeLogLikelihoodDerivative只需要判断后两个参数的维度是否一致
    */
    template <>
    class OperOrganizer<TernaryOpTags::NegativeLogLikelihoodDerivative, CategoryTags::Matrix>
    {
    private:
        size_t rowNum_;
        size_t colNum_;

    public:
        template <typename tP1, typename tP2, typename tP3>
        OperOrganizer(const tP1 &tp1, const tP2 &tp2, const tP3 &tp3) : rowNum_(tp2.RowNum()), colNum_(tp2.ColNum())
        {
            assert((tp2.RowNum() == tp3.RowNum()));
            assert((tp2.ColNum() == tp3.ColNum()));
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

    template <>
    class OperOrganizer<TernaryOpTags::NegativeLogLikelihoodDerivative, CategoryTags::MatrixBatch>
    {
    private:
        size_t rowNum_;
        size_t colNum_;
        size_t batchNum_;

    public:
        template <typename tP1, typename tP2, typename tP3>
        OperOrganizer(const tP1 &tp1, const tP2 &tp2, const tP3 &tp3) : rowNum_(tp2.RowNum()), colNum_(tp2.ColNum()), batchNum_(tp2.BatchNum())
        {
            assert((tp2.RowNum() == tp3.RowNum()));
            assert((tp2.ColNum() == tp3.ColNum()));
            assert((tp2.BatchNum() == tp3.BatchNum()));
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
            return batchNum_;
        }
    };

    template<typename tP1, typename tP2, typename tP3>
    struct OperElementType_<TernaryOpTags::NegativeLogLikelihoodDerivative, tP1, tP2, tP3>
    {
        using ElementType = typename tP2::ElementType;
    };

    template<typename tP1, typename tP2, typename tP3>
    struct OperDeviceType_<TernaryOpTags::NegativeLogLikelihoodDerivative, tP1, tP2, tP3>
    {
        using DeviceType = typename tP2::DeviceType;
    };

    /*
    * OperXXX_
    * 首先判断输入参数类型是否合法
    */
    template<typename tP1, typename tP2, typename tP3>
    struct OperNegativeLogLikelihoodDerivative_
    {
    private:
        using RawType1 = RemConstRef<tP1>;
        using RawType2 = RemConstRef<tP2>;
        using RawType3 = RemConstRef<tP3>;

    public:
        static constexpr bool valid = (IsScalar<RawType1> && IsMatrix<RawType2> && IsMatrix<RawType3>) ||
                                      (IsScalarBatch<RawType1> && IsMatrixBatch<RawType2> && IsMatrixBatch<RawType3>);

        static auto Eval(tP1 && tp1, tP2 &&tp2, tP3 &&tp3)
        {
            static_assert(std::is_same<typename RawType2::ElementType, typename RawType3::ElementType>::value,
            "Matrices with different element types cannot negative log likelihood derivative directly");
            static_assert(std::is_same<typename RawType2::DeviceType, typename RawType3::DeviceType>::value,
            "Matrices with different device types cannot negative log likelihood derivative directly");

            using ResType = TernaryOp<TernaryOpTags::NegativeLogLikelihoodDerivative, RawType1, RawType2, RawType3>;
            return ResType(std::forward<tP1>(tp1), std::forward<tP2>(tp2), std::forward<tP3>(tp3));
        }
    };

    template<typename tP1, typename tP2, typename tP3,
    std::enable_if_t<OperNegativeLogLikelihoodDerivative_<tP1, tP2, tP3>::valid>* =nullptr>
    auto NegatveLogLikehoodDerivative(tP1 &&tp1, tP2 &&tp2, tP3 &&tp3)
    {
        return OperNegativeLogLikelihoodDerivative_<tP1, tP2, tP3>::Eval(std::forward<tP1>(tp1), std::forward<tP2>(tp2), std::forward<tP3>(tp3));
    }

}