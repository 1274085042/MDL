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
    struct OperDot_
    {
    private:
        using RawType1 = RemConstRef<tP1>;
        using RawType2 = RemConstRef<tP2>;
    
    public:
        static constexpr bool valid = (IsMatrix<RawType1> && IsMatrix<RawType2>) ||
                                (IsMatrix<RawType1> && IsMatrixBatch<RawType2>) ||
                                (IsMatrixBatch<RawType1> && IsMatrix<RawType2>) ||
                            (IsMatrixBatch<RawType1> && IsMatrixBatch<RawType2>);
        
        template<typename T1, typename T2, std::enable_if_t<std::is_same<T1, T2>::value> * = nullptr>  
        static auto Eval(tP1 &&tp1, tP2 &&tp2)
        {
            static_assert(std::is_same<typename RawType1::ElementType, typename RawType2::ElementType>::value,
            "Matrices with different element type cannot dot directly");
            static_assert(std::is_same<typename RawType1::DeviceType, typename RawType2::DeviceType>::value,
            "Matrices with different device type cannot dot directly");

            using ResType = BinaryOp<BinaryOpTags::Dot, RawType1, RawType2>;
            return ResType(std::forward<tP1>(tp1), std::forward<tP2>(tp2));
        }

        template<typename T1, typename T2,
        std::enable_if_t<std::is_same<T1, CategoryTags::Matrix>::value>* = nullptr,
        std::enable_if_t<std::is_same<T2, CategoryTags::MatrixBatch>::value> * = nullptr>
        static auto Eval(tP1 &&tp1, tP2 &&tp2)
        {
            static_assert(std::is_same<typename RawType1::ElementType, typename RawType2::ElementType>::value,
            "Matrices with different element type cannot dot directly");
            static_assert(std::is_same<typename RawType1::DeviceType, typename RawType2::DeviceType>::value,
            "Matrices with different device type cannot dot directly");

            Duplicate<RawType1> tmp(std::forward<tP1>(tp1), tp2.BatchNum());
            using ResType = BinaryOp<BinaryOpTags::Dot, Duplicate<RawType1>, RawType2>;
            return ResType(std::move(tmp), std::forward<tP2>(tp2));
        }

        template<typename T1, typename T2,
        std::enable_if_t<std::is_same<T1, CategoryTags::MatrixBatch>::value> * =nullptr,
        std::enable_if_t<std::is_same<T2, CategoryTags::Matrix>::value> * = nullptr>
        static auto Eval(tP1 &&tp1, tP2 &&tp2)
        {
            return OperDot_<tP2, tP1>::template Eval<T2, T1>(std::forward<tP2>(tp2), std::forward<tP1>(tp1));
        }
    };

    template<typename tP1, typename tP2, 
    std::enable_if_t<OperDot_<tP1, tP2>::valid> * = nullptr>
    auto dot(tP1 && tp1, tP2 && tp2)
    {
        return OperDot_<tP1, tP2>::template Eval<DataCategory<tP1>, DataCategory<tP2>>(std::forward<tP1>(tp1), std::forward<tP2>(tp2));
    }
}