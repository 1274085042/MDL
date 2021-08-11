#pragma once

#include "operators.h"
#include "./facilities/tags.h"
#include "../facilities/traits.h"
#include "../data_type/batch/duplicate.h"
#include "../data_type/facilities/traits.h"
#include "../data_type/matrices/trival_matrix.h"
#include <type_traits>

namespace MDL
{
    template<typename tP1, typename tP2>
    struct OperElementMul_  
    {
    private:
        using RawType1 = RemConstRef<tP1>;
        using RawType2 = RemConstRef<tP2>;

    public:
        static constexpr bool valid = (IsMatrix<RawType1> && IsMatrix<RawType2>) ||
                                      (IsMatrix<RawType1> && IsScalar<RawType2>) ||
                                      (IsScalar<RawType1> && IsMatrix<RawType2>) ||
                                 (IsMatrix<RawType1> && IsMatrixBatch<RawType2>) ||
                                 (IsMatrixBatch<RawType1> && IsMatrix<RawType2>) ||
                            (IsMatrixBatch<RawType1> && IsMatrixBatch<RawType2>) ||
                                 (IsMatrixBatch<RawType1> && IsScalar<RawType2>) ||
                                 (IsScalar<RawType1> && IsMatrixBatch<RawType2>);

        template<typename T1, typename T2, 
        std::enable_if_t<std::is_same<T1, T2>::value> * = nullptr>
        static auto Eval(tP1 &&tp1, tP2 &&tp2)
        {
            static_assert(std::is_same<typename RawType1::ElementType, typename RawType2::ElementType>::valid, 
            "Matrices with different element types cannot multiply directly");
            static_assert(std::is_same<typename RawType1::DeviceType, typename RawType2::DeviceType>::valid, 
            "Matrices with different device types cannot multiply directly");

            using ResType = BinaryOp<BinaryOpTags::ElementMul, RawType1, RawType2>;

            return ResType(std::forward<tP1>(tp1), std::forward<tP2>(tp2));   
        }

        template<typename T1, typename T2,
        std::enable_if_t<std::is_same<T1, CategoryTags::Scalar>::value> * = nullptr,
        std::enable_if_t<std::is_same<T2, CategoryTags::Matrix>::value> * = nullptr>
        static auto Eval(tP1 && tp1, tP2 && tp2)
        {
            using ElementType = typename RawType2::ElementType;
            using DeviceType = typename RawType2::DeviceType;

            auto tmp = MakeTrivalMatrix<ElementType, DeviceType>(tp2.RowNum(), tp2.ColNum(), std::forward<tP1>(tp1));

            using ResType = BinaryOp<BinaryOpTags::ElementMul, RemConstRef<decltype(tmp)>, RawType2>;
            return ResType(std::move(tmp), std::forward<tP2>(tp2));
        }

        template<typename T1, typename T2,
        std::enable_if_t<std::is_same<T1, CategoryTags::Matrix>::value> * = nullptr,
        std::enable_if_t<std::is_same<T2, CategoryTags::Scalar>::value> * = nullptr>
        static auto Eval(tP1 && tp1, tP2 && tp2)
        {
            return OperElementMul_<tP2, tP1>::template Eval<T2, T1>(std::forward<tP2>(tp2), std::forward<tP1>(tp1));
        }

        template<typename T1, typename T2, 
        std::enable_if_t<std::is_same<T1, CategoryTags::Matrix>::value> * = nullptr,
        std::enable_if_t<std::is_same<T2, CategoryTags::MatrixBatch>::value> * = nullptr>
        static auto Eval(tP1 && tp1, tP2 tp2)
        {
            static_assert(std::is_same<typename RawType1::ElementType, typename RawType2::ElementType>::valid, 
            "Matrices with different element types cannot multiply directly");
            static_assert(std::is_same<typename RawType1::DeviceType, typename RawType2::DeviceType>::valid, 
            "Matrices with different device types cannot multiply directly");

            Duplicate<RawType1> tmp(std::forward<tP1>(tp1), tp2.BatchNum());

            using ResType = BinaryOp<BinaryOpTags::ElementMul, Duplicate<RawType1>, RawType2>;
            return ResType(std::move(tmp), std::forward<tP2>(tp2));
        }

        template<typename T1, typename T2,
        std::enable_if_t<std::is_same<T1, CategoryTags::MatrixBatch>::value> * = nullptr,
        std::enable_if_t<std::is_same<T2, CategoryTags::Matrix>::value> * = nullptr>
        static auto Eval(tP1 && tp1, tP2 && tp2)
        {
            return OperElementMul_<tP2, tP1>::template Eval<T2, T1>(std::forward<tP2>(tp2), std::forward<tP1>(tp1));
        }

        template <typename T1, typename T2,
        std::enable_if_t<std::is_same<T1, CategoryTags::Scalar>::value> * = nullptr,
        std::enable_if_t<std::is_same<T2, CategoryTags::MatrixBatch>::value> * = nullptr>
        static auto Eval(tP1 && tp1, tP2 && tp2)
        {          
            using ElementType = typename RawType2::ElementType;
            using DeviceType = typename RawType2::DeviceType;

            auto tmpMatrix = MakeTrivalMatrix<ElementType, DeviceType>(tp2.RowNum(), tp2.ColNum(), std::forward<tP1>(tp1));

            //Duplicate<RemConstRef<decltype<tmpMatrix>>> tmp{std::forward<tP1> (tp1), tp2.BatchNum()};

            auto tmp = MakeDuplicate(tp2.BatchNum(), std::move(tmpMatrix));

            using ResType = BinaryOp<BinaryOpTags::ElementMul, RemConstRef<decltype(tmp)>, RawType2>;
            return ResType(std::move(tmp), std::forward<tP2>(tp2));
        }

        template<typename T1, typename T2,
        std::enable_if_t<std::is_same<T1, CategoryTags::MatrixBatch>::value> * = nullptr,
        std::enable_if_t<std::is_same<T2, CategoryTags::Scalar>::value> * = nullptr>
        static auto Eval(tP1 && tp1, tP2 && tp2)
        {
            return OperElementMul_<tP2, tP1>::template Eval<T2, T1>(std::forward<tP2>(tp2), std::forward<tP1>(tp1));
        }
    };

    template<typename tP1, typename tP2>
    auto operator * (tP1 &&tp1 , tP2 &&tp2)
    {
        return OperElementMul_<tP1,tP2>::template Eval<DataCategory<tP1>, DataCategory<tP2>>(std::forward<tP1>(tp1), std::forward<tP2>(tp2));
    }
}