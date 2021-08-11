#pragma once

#include "operators.h"
#include "./facilities/tags.h"
#include "../facilities/traits.h"
#include "../data_type/facilities/traits.h"
#include <type_traits>

namespace MDL
{
    template <typename tP1, typename tP2, typename tP3>
    struct OperInterpolate_
    {
    private:
        using RawType1 = RemConstRef<tP1>;
        using RawType2 = RemConstRef<tP2>;
        using RawType3 = RemConstRef<tP3>;
    
    public:
        static constexpr bool valid = (IsMatrix<RawType1> && IsMatrix<RawType2> && IsMatrix<RawType3>) ||
                        (IsMatrixBatch<RawType1> && IsMatrixBatch<RawType2> && IsMatrixBatch<RawType3>);
        
        static auto Eval(tP1 &&tp1, tP2 &&tp2, tP3 &&tp3)
        {
            static_assert(std::is_same<typename RawType1::ElementType, typename RawType2::ElementType>::value, 
            "Matrices with different element types cannot interpolate directly");
            static_assert(std::is_same<typename RawType1::DeviceType, typename RawType2::DeviceType>::value, 
            "Matrices with different device types cannot interpolate directly");

            static_assert(std::is_same<typename RawType1::ElementType, typename RawType3::ElementType>::value, 
            "Matrices with different element types cannot interpolate directly");
            static_assert(std::is_same<typename RawType1::DeviceType, typename RawType3::DeviceType>::value, 
            "Matrices with different device types cannot interpolate directly");

            using ResType = TernaryOp<TernaryOpTags::Interpolate, RawType1, RawType2, RawType3>;
            return ResType(std::forward<tP1>(tp1), std::forward<tP2>(tp2), std::forward<tP3>(tp3));
        }
    };

    template<typename tP1, typename tP2, typename tP3, 
    std::enable_if_t<OperInterpolate_<tP1, tP2, tP3>::valid> * = nullptr>
    auto Interpolate(tP1 &&tp1, tP2 &&tp2, tP3 &&tp3)
    {
        return OperInterpolate_<tP1, tP2, tP3>::Eval(std::forward<tP1>(tp1), std::forward<tP2>(tp2), std::forward<tP3>(tp3));
    }
}