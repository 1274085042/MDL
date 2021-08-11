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
    struct OperDivide_
    {
    private:
        using RawType1 = RemConstRef<tP1>;
        using RawType2 = RemConstRef<tP2>;

    public:
        static constexpr bool valid = (IsMatrix<RawType1> && IsMatrix<RawType2>) ||
                                      (IsScalar<RawType1> && IsMatrix<RawType2>) ||
                                      (IsMatrix<RawType1> && IsScalar<RawType2>) ||
                            (IsMatrixBatch<RawType1> && IsMatrixBatch<RawType2>) ||
                                 (IsMatrix<RawType1> && IsMatrixBatch<RawType2>) ||
                                 (IsMatrixBatch<RawType1> && IsMatrix<RawType2>) ||
                                 (IsScalar<RawType1> && IsMatrixBatch<RawType2>) ||
                                 (IsMatrixBatch<RawType1> && IsScalar<RawType2>) ;

        template<typename T1, typename T2, std::enable_if_t<std::is_same<T1, T2>::value> * = nullptr>
        static auto Eval(tP1 &&tp1, tP2 &&tp2)
        {
            static_assert(std::is_same<typename RawType1::ElementType, typename RawType2::ElementType>::value, 
                "Matrices with different element types cannot divide directly");
            static_assert(std::is_same<typename RawType1::DeviceType, typename RawType2::DeviceType>::value,
                "Matrices with different device types cannot divide directly");
            
            using ResType = BinaryOp<BinaryOpTags::Divide, RawType1, RawType2>;
            return ResType(std::forward<tP1>(tp1), std::forward<tP2>(tp2))
        }

        template<typename T1, typename T2, 
        std::enable_if_t<std::is_same<CategoryTags::Matrix, T1>::value> * = nullptr,
        std::enable_if_t<std::is_same<CategoryTags::Scalar, T2>::value> * = nullptr>
        static auto Eval(tP1 &&tp1, tP2 &&tp2)
        {
            using ElementType = typename RawType1::ElementType;
            using DeviceType = typename RawType2::DeviceType;

            auto tmp = MakeTrivalMatrix<ElementType, DeviceType>(tp1.RowNum(), tp1.ColNum(), std::forward<tP2>(tp2));
           
            using ResType = BinaryOp<BinaryOpTags::Divide, RawType1, RemConstRef<decltype(tmp)>>;
            return ResType(std::forward<tP1>(tp1), std::move(tmp)); 
        }

        template<typename T1, typename T2,
        std::enable_if_t<std::is_same<CategoryTags::Scalar, T1>::value> * = nullptr,
        std::enable_if_t<std::is_same<CategoryTags::Matrix, T2>::value> * = nullptr>
        static auto Eval(tP1 &&tp1, tP2 &&tp2)
        {
            return OperDivide_<tP2, tP1>::template Eval<T2, T1>(std::forward<tP2> (tp2), std::forward<tP1>(tp1));
        }

        template<typename T1, typename T2,
        std::enable_if_t<std::is_same<CategoryTags::Matrix, T1>::value> * = nullptr,
        std::enable_if_t<std::is_same<CategoryTags::MatrixBatch, T2>::value> * = nullptr>
        static auto Eval(tP1 &&tp1, tP2 &&tp2)
        {
            static_assert(std::is_same<typename RawType1::ElementType, typename RawType2::ElementType>::value, 
            "Matrices with different element types cannot divide directly");
            static_assert(std::is_same<typename RawType1::DeviceType, typename RawType2::DeviceType>::value, 
            "Matrices with different device types cannot divide directly");

            auto tmp = MakeDuplicate(tp2.BatchNum(), std::forward<tP1>(tp1));

            using ResType = BinaryOp<BinaryOpTags::Divide, RemConstRef<decltype(tmp)>, RawType2>;

            return ResType(std::move(tmp), std::forward<tP2>(tp2));
        }

        template<typename T1, typename T2,
        std::enable_if_t<std::is_same<CategoryTags::MatrixBatch, T1>::value> * = nullptr,
        std::enable_if_t<std::is_same<CategoryTags::Matrix, T2>::value> * = nullptr>
        static auto Eval(tP1 &&tp1, tP2 &&tp2)
        {
            return OperDivide_<tP2, tP1>::template Eval<T2, T1>(std::forward<tP2> (tp2), std::forward<tP1>(tp1));
        }

        template<typename T1, typename T2, 
        std::enable_if_t<std::is_same<CategoryTags::Scalar, T1>::value> * = nullptr,
        std::enable_if_t<std::is_same<CategoryTags::MatrixBatch, T2>::value> * = nullptr>
        static auto Eval(tP1 &&tp1, tP2 &&tp2)
        {
            using ElementType = typename RawType2::ElementType;
            using DeviceType  = typename RawType2::DeviceType;

            auto tmpMatrix = MakeTrivalMatrix<ElementType, DeviceType>(tp2.RowNum(), tp2.ColNum(), std::forward<tP1>(tp1));;
            auto tmpMatrixBatch = MakeDuplicate(tp2.BatchNum(), std::move(tmpMatrix));

            using ResType = BinaryOp<BinaryOpTags::Divide, RemConstRef<decltype(tmpMatrixBatch)>, RawType2>;
            return ResType(std::move(tmpMatrixBatch), std::forward<tP2>(tp2));
        }

        template<typename T1, typename T2,
        std::enable_if_t<std::is_same<CategoryTags::MatrixBatch, T1>::value> * = nullptr,
        std::enable_if_t<std::is_same<CategoryTags::Matrix, T2>::value> * = nullptr>
        static auto Eval(tP1 &&tp1, tP2 &&tp2)
        {
            return OperDivide_<tP2, tP1>::template Eval<T2, T1>(std::forward<tP2> (tp2), std::forward<tP1>(tp1));
        }
    };

    template<typename tP1, typename tP2, std::enable_if_t<OperDivide_<tP1, tP2>::valid> * = nullptr>
    auto operator/ (tP1 &&tp1, tP2 &&tp2)
    {
        return OperDivide_<tP1, tP2>::template Eval<DataCategory<tP1>, DataCategory<tP2>>(std::forward<tP1>(tp1), std::forward<tP2>(tp2));
    }
}