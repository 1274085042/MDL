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
    template <typename tP1, typename tP2>
    struct OperSubstract_
    {
    private:
        using RawType1 = typename RemConstRef<tP1>;
        using RawType2 = typename RemConstRef<tP2>;

    public:
        static constexpr bool valid = (IsMatrix<RawType1> && IsMatrix<RawType2>) ||
                                      (IsMatrix<RawType1> && IsScalar<RawType2>) ||
                                      (IsScalar<RawType1> && IsMatrix<RawType2>) ||
                            (IsMatrixBatch<RawType1> && IsMatrixBatch<RawType2>) ||
                                 (IsMatrixBatch<RawType1> && IsMatrix<RawType2>) ||
                                 (IsMatrix<RawType1> && IsMatrixBatch<RawType2>) ||
                                 (IsMatrixBatch<RawType1> && IsScalar<RawType2>) ||
                                 (IsScalar<RawType1> && IsMatrixBatch<RawType2>);

        template<typename T1, typename T2, 
        std::enable_if_t<std::is_same<T1, T2>::value> * = nullptr>
        static auto Eval(tP1 &&tp1, tP2 &&tp2)
        {
            static_assert(std::is_same<typename RawType1::ElementType, typename RawType2::ElementType>::value, 
            "Matrices with different element types cannot substract directly");
            static_assert(std::is_same<typename RawType1::DeviceType, typename RawType2::DeviceType>::value, 
            "Matrices with different device types cannot substract directly");

            using ResType = BinaryOp<BinaryOpTags::Substract, RawType1, RawType2>;
            return ResType(std::forward<tP1>(tp1), std::forward<tP2>(tp2));
        }

        template<typename T1, typename T2, 
        std::enable_if_t<std::is_same<T1, CategoryTags::Matrix>::value> * = nullptr,
        std::enable_if_t<std::is_same<T2, CategoryTags::Scalar>::value> * = nullptr>
        static auto Eval(tP1 &&tp1, tP2 &&tp2)
        {
            using ElementType = typename RawType1::ElementType;
            using DeviceType = typename RawType1::DeviceType;

            auto tmpMatrix = MakeTrivalMatrix<ElementType, DeviceType>(tp1.RowNum(), tp1.ColNum(), std::forward<tP2>(tp2));
            using ResType = BinaryOp<BinaryOpTags::Susbtract, RawType1, RemConstRef<decltype(tmpMatrix)>>;
            return ResType(std::forward<tP1>(tp1), std::move(tmpMatrix));
        }
        
        template<typename T1, typename T2, 
        std::enable_if_t<std::is_same<T1, CategoryTags::Scalar>::value> * = nullptr,
        std::enable_if_t<std::is_same<T2, CategoryTags::Matrix>::value> * = nullptr>
        static auto Eval(tP1 &&tp1, tP2 &&tp2)
        {
            return OperSubstract_<tP2, tP1>::template Eval<T2, T1>(std::forward<tP2>(tp2), std::forward<tP1>(tp1));
        }

        template<typename T1, typename T2, 
        std::enable_if_t<std::is_same<T1, CategoryTags::MatrixBatch>::value> * = nullptr,
        std::enable_if_t<std::is_same<T2, CategoryTags::Scalar>::value> * = nullptr>
        static auto Eval(tP1 &&tp1, tP2 &&tp2)
        {
            using ElementType = typename RawType1::ElementType;
            using DeviceType = typename RawType1::DeviceType;

            auto tmpMatrix = MakeTrivalMatrix<ElementType, DeviceType>(tp1.RowNum(), tp1.ColNum(), std::forward<tP2>(tp2));
            auto tmpMatrixBatch = MakeDuplicate(tp1.BatchNum(), std::move(tmpMatrix));
            
            using ResType = BinaryOp<BinaryOpTags::Substract, RawType1, RemConstRef<decltype(tmpMatrixBatch)>>;
            return ResType(std::forward<tP1>(tp1), std::move(tmpMatrixBatch));
        }
        
        template<typename T1, typename T2, 
        std::enable_if_t<std::is_same<T1, CategoryTags::Scalar>::value> * = nullptr,
        std::enable_if_t<std::is_same<T2, CategoryTags::MatrixBatch>::value> * = nullptr>
        static auto Eval(tP1 &&tp1, tP2 &&tp2)
        {
            return OperSubstract_<tP2, tP1>::template Eval<T2, T1>(std::forward<tP2>(tp2), std::forward<tP1>(tp1));
        }

        template<typename T1, typename T2, 
        std::enable_if_t<std::is_same<T1, CategoryTags::MatrixBatch>::value> * = nullptr,
        std::enable_if_t<std::is_same<T2, CategoryTags::Matrix>::value> * = nullptr>
        static auto Eval(tP1 &&tp1, tP2 &&tp2)
        {
            static_assert(std::is_same<typename RawType1::ElementType, typename RawType2::ElementType>::value,
            "Matrices with different element types cannot substract directly");
            static_assert(std::is_same<typename RawType1::DeviceType, typename RawType2::DeviceType>::value,
            "Matrices with different device types cannot substract directly");

            auto tmpMatrixBatch = MakeDuplicate(tp1.BatchNum(), std::forward<tP2>(tp2));
            
            using ResType = BinaryOp<BinaryOpTags::Susbtract, RawType1, RemConstRef<decltype(tmpMatrixBatch)>>;
            return ResType(std::forward<tP1>(tp1), std::move(tmpMatrixBatch));
        }
        
        template<typename T1, typename T2, 
        std::enable_if_t<std::is_same<T1, CategoryTags::Matrix>::value> * = nullptr,
        std::enable_if_t<std::is_same<T2, CategoryTags::MatrixBatch>::value> * = nullptr>
        static auto Eval(tP1 &&tp1, tP2 &&tp2)
        {
            return OperSubstract_<tP2, tP1>::template Eval<T2, T1>(std::forward<tP2>(tp2), std::forward<tP1>(tp1));
        }        
    };

    template<typename tP1, typename tP2, std::enable_if_t<OperSubstract_<tP1, tP2>::valid> * = nullptr>
    auto operator-(tP1 &&tp1, tP2 &&tp2)
    {
        return OperSubstract_<tP1, tP2>::template Eval<DataCategory<tP1>, DataCategory<tP2>>(std::forward<tP1>(tp1), std::forward<tP2>(tp2));
    }
}