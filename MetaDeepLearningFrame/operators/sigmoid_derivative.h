#pragma once

#include "operators.h"
#include "./facilities/tags.h"
#include "./facilities/traits.h"
#include "../facilities/traits.h"
#include "../data_type/facilities/traits.h"
#include <type_traits>

namespace MDL
{
    template <typename tP1, typename tP2>
    struct OperSigmoidDerivative_
    {
    private:
        using RawType1 = RemConstRef<tP1>;
        using RawType2 = RemConstRef<tP2>;
    
    public:
        static constexpr bool valid = (IsMatrix<RawType1> && IsMatrix<RawType2>) ||
                            (IsMatrixBatch<RawType1> && IsMatrixBatch<RawType2>);
        
        static auto Eval(tP1 &&tp1, tP2 &&tp2)
        {
            static_assert(std::is_same<typename RawType1::ElementType, typename RawType2::ElementType>::value,
            "Matrices with different element type cannot sigmoid derivative");
            static_assert(std::is_same<typename RawType1::ElementType, typename RawType2::ElementType>::value,
            "Matrices with different device type cannot sigmoid derivative");
            
            using ResType = BinaryOp<BinaryOpTags::SigmoidDerivative, RawType1, RawType2>;
            return ResType(std::forward<tP1>(tp1), std::forward<tP2>(tp2));
        }

    };

    template<typename tP1, typename tP2, std::enable_if_t<OperSigmoidDerivative_<tP1,tP2>::valid> * =nullptr>
    auto SigmoidDerivative(tP1 &&tp1, tP2 &&tp2)
    {
        return OperSigmoidDerivative_<tP1, tP2>::Eval(std::forward<tP1>(tp1), std::forward<tP2>(tp2));
    }

}