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
    template<typename tGrad, typename tOut>
    struct OperTanhDrivative_
    {
    private:
        using RawType1 = RemConstRef<tGrad>;
        using RawType2 = RemConstRef<tOut>;

    public:
        static constexpr bool valid = (IsMatrix<RawType1> && IsMatrix<RawType2>) ||
                            (IsMatrixBatch<RawType1> && IsMatrixBatch<RawType2>);

        static auto Eval(tGrad &&grad, tOut &&out)
        {
            using ResType = BinaryOp<BinaryOpTags::TanhDerivative, RawType1, RawType2>;
            return ResType(std::forward<tGrad>(grad), std::forward<tOut>(out));
        }
    };

    template<typename tGrad, typename tOut, std::enable_if_t<OperTanhDrivative_<tGrad, tOut>::valid> * = nullptr>
    auto TanhDerivative(tGrad &&grad, tOut &&out)
    {
        return OperTanhDrivative_<tGrad, tOut>::Eval(std::forward<tGrad>(grad), std::forward<tOut>(out));
    }
}